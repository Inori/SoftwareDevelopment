#include <Windows.h>
#include <stdio.h>
#include <string>

#include "pngfile.h"
using namespace std;

#pragma pack(1)

typedef struct tex_header_s
{
	char sig[8]; //Texture
	DWORD data_size;
} tex_header_t;

typedef struct tex_info_s
{
	DWORD uk0; //not sure, maybe version or bit_depth ...it afects the pic position in game window
	DWORD png_size;
	DWORD width;
	DWORD height;
	DWORD width2;
	DWORD height2;
}tex_info_t;

typedef struct lz77_header_s
{
	char sig[4]; //LZ77
	DWORD decompress_len;
	DWORD compress_len;
	DWORD second_part_off;

} lz77_header_t;

typedef struct png_header_s
{
	char sig[4]; //\x89PNG
} png_header_t;

#pragma pack()


void lz77_compress(unsigned char *&out_buff, unsigned long& out_len, unsigned char* in_buff, unsigned long width, unsigned long height)
{
	unsigned long graph_len = (width * height * 4);
	unsigned long zeros = graph_len * 2 / 0x10;
	unsigned long len = zeros + graph_len;
	out_len = len;
	unsigned char *buff = new unsigned char[len];
	memset(buff, 0, zeros);
	memcpy(buff + zeros, in_buff, graph_len);
	out_buff = buff;
}

DWORD big_endian(DWORD n)
{
	DWORD res = 0;
	BYTE *p = (BYTE*)&n;
	res = p[0] * 0x1000000 + p[1] * 0x10000 + p[2] * 0x100 + p[3];
	return res;
}


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("ps3tex2pctex v1.0 by 福音\n");
		printf("usage: %s <input.tex> \n", argv[0]);
		return -1;
	}
	string in_filename(argv[1]);

	FILE *fin = fopen(in_filename.c_str(), "rb");
	if (!fin)
		return -1;

	fseek(fin, 0, SEEK_END);
	DWORD size = ftell(fin);
	fseek(fin, 0, SEEK_SET);

	tex_header_t htex;
	fread(&htex, sizeof(tex_header_t), 1, fin);

	//如果不是有Texture文件头的外部tex文件，就是MP里的TEX_ALL.TEX，暂时先这样判断
	if (strncmp(htex.sig, "Texture", 7))
	{
		fseek(fin, 0, SEEK_SET);
	}

	//获取文件长宽、大小信息
	tex_info_t hinfo;
	fread(&hinfo, sizeof(tex_info_t), 1, fin);

	//检测png文件头
	png_header_t hpng;
	fread(&hpng, sizeof(png_header_t), 1, fin);
	if (memcmp(hpng.sig, "\x89PNG", 4))
	{
		printf("wrong png header found!\n");
		return -1;
	}
	fseek(fin, 0-sizeof(png_header_t), SEEK_CUR);

	//写png文件
	unsigned long png_size = big_endian(hinfo.png_size);
	unsigned char *png_buff = new unsigned char[png_size];
	fread(png_buff, png_size, 1, fin);

	string fn_tmp = in_filename + ".png";
	FILE *png_temp = fopen(fn_tmp.c_str(), "wb");
	if (!png_temp)
		return -1;
	fwrite(png_buff, png_size, 1, png_temp);
	delete[] png_buff;
	fclose(png_temp);

	//读png文件
	pic_data png;
	read_png_file(fn_tmp, &png);

	//伪压缩数据
	unsigned char *lz77_data = NULL;
	unsigned long len = 0;
	lz77_compress(lz77_data, len, png.rgba, png.width, png.height);

	//写转换后文件
	string out_filename = "pc_" + in_filename;
	FILE *fout = fopen(out_filename.c_str(), "wb");
	if (!fout)
		return -1;

	if (!strncmp(htex.sig, "Texture", 7))
	{
		htex.data_size = big_endian(len + sizeof(lz77_header_t)+0x24);
		fwrite(&htex, sizeof(tex_header_t), 1, fout);
	}

	hinfo.png_size = big_endian(len + sizeof(lz77_header_t));
	hinfo.uk0 = big_endian(8);
	fwrite(&hinfo, sizeof(tex_info_t), 1, fout);

	lz77_header_t hlz;
	strcpy(hlz.sig, "LZ77");
	hlz.compress_len = big_endian(png.width * png.height * 4);
	hlz.decompress_len = hlz.compress_len;
	hlz.second_part_off = big_endian(big_endian(hlz.compress_len) * 2 / 0x10 + 0x10);

	fwrite(&hlz, sizeof(lz77_header_t), 1, fout);
	fwrite(lz77_data, len, 1, fout);
	delete[]lz77_data;

	//原版文件末尾有Part结构，尚未分析清楚，但是实验把Part部分删掉也不会有影响，所以这部分暂时补0
	unsigned long zeros = 0x24;
	unsigned char *z_buff = new unsigned char[zeros];
	memset(z_buff, 0, zeros);
	fwrite(z_buff, zeros, 1, fout);
	delete[] z_buff;

	fclose(fout);
	fclose(fin);
	return 0;
}