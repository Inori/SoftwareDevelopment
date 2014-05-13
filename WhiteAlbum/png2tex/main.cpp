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
		printf("png2tex v1.0 by 福音\n");
		printf("usage: %s <input.png> \n", argv[0]);
		return -1;
	}
	string png_filename(argv[1]);

	//读png文件
	pic_data png;
	read_png_file(png_filename, &png);

	//伪压缩数据
	unsigned char *lz77_data = NULL;
	unsigned long len = 0;
	lz77_compress(lz77_data, len, png.rgba, png.width, png.height);

	//写转换后文件
	string out_filename = png_filename.substr(0,png_filename.find_last_of(".")) + ".tex";
	FILE *fout = fopen(out_filename.c_str(), "wb");
	if (!fout)
		return -1;
	tex_header_t htex;
	strcpy(htex.sig, "Texture ");
	htex.data_size = big_endian(len + sizeof(lz77_header_t)+0x24);
	fwrite(&htex, sizeof(tex_header_t), 1, fout);

	tex_info_t hinfo;
	hinfo.uk0 = big_endian(8);
	hinfo.png_size = big_endian(len + sizeof(lz77_header_t));
	hinfo.width = big_endian(png.width);
	hinfo.height = big_endian(png.height);
	hinfo.width2 = big_endian(png.width);
	hinfo.height2 = big_endian(png.height);
	
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
	return 0;
}