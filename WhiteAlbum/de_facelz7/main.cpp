#include <Windows.h>
#include <stdio.h>
#include <string>

using namespace std;

#pragma pack(1)

typedef struct tex_header_s
{
	char sig[8]; //Texture
	DWORD data_size;
	DWORD bit_depth; //not sure, maybe...
	DWORD compress_len;
	DWORD width;
	DWORD height;
	DWORD width2;
	DWORD height2;
} tex_header_t;

typedef struct ps3_uk_header_s
{
	DWORD uk0;
	DWORD uk1;
	DWORD width;
	DWORD height;
}ps3_uk_header_t;

typedef struct lz77_header_s
{
	char sig[4]; //LZ77
	DWORD decompress_len;
	DWORD compress_len;
	DWORD second_part_off;

} lz77_header_t;

#pragma pack()

BYTE flag = 0;
BYTE* pout;
BYTE* in_first;
BYTE* in_second;
DWORD in_len;

void lz77_decompress()
{
	__asm
	{
		push ebp;
		xor     ebx, ebx;
		mov     ecx, in_second;
		mov     edx, in_len;
		mov     ebp, pout;
		cld;
Loop1:
		test    ebx, ebx
		jnz     Tag1
		mov     esi, in_first
		mov     al, byte ptr[esi]
		inc     esi
		mov     flag, al
		mov     ebx, 0x8
		mov     in_first, esi
Tag1 :
		test    flag, 0x80
		je      Tag2
		movzx   eax, byte ptr[ecx]
		movzx   esi, byte ptr[ecx + 0x1]
		shl     eax, 0x8
		or      eax, esi
		movzx   edi, al
		shr     eax, 0x8
		mov     esi, ebp
		add     edi, 0x3
		sub     esi, eax
		test    edi, edi
		jbe     Tag3
			//lea     esp, dword ptr[esp]
Loop2 :
		mov     al, byte ptr[esi]
		mov     byte ptr[ebp], al
		inc     ebp
		inc     esi
		sub     edi, 0x1
		jnz     Loop2
Tag3 :
		add     ecx, 0x2
		jmp     Tag4
Tag2 :
		mov     al, byte ptr[ecx]
		mov     byte ptr[ebp], al
		inc     ebp
		inc     ecx
Tag4 :
		shl     flag, 1
		dec     ebx
		sub     edx, 0x1
		jnz     Loop1

		pop ebp
	}
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
		printf("de_facelz7 v1.0 by 福音\n");
		//如果没有输出文件名，默认输出文件名为输入文件名加.bin后缀
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


	lz77_header_t hlz;
	fread(&hlz, sizeof(lz77_header_t), 1, fin);
	if (strncmp(hlz.sig, "LZ77", 4))
		return -1;

	DWORD first_len = big_endian(hlz.second_part_off) - sizeof(lz77_header_t);
	BYTE *first_data = new BYTE[first_len];
	fread(first_data, first_len, 1, fin);


	DWORD second_len = size - (first_len + sizeof(lz77_header_t));
	BYTE *second_data = new BYTE[second_len];
	fread(second_data, second_len, 1, fin);

	DWORD out_len = big_endian(hlz.decompress_len);
	BYTE *out_data = new BYTE[out_len];
	memset(out_data, 0, out_len);
	DWORD step = big_endian(hlz.compress_len);

	//这里内联汇编使用了ebp，所以用全局变量传参，暂时没想到好办法
	pout = out_data;
	in_first = first_data;
	in_second = second_data;
	in_len = step;
	lz77_decompress();


	string out_fname;
	if (argc > 2)
	out_fname = argv[2];
	else
	out_fname = in_filename + ".bin";

	FILE *fout = fopen(out_fname.c_str(), "wb");
	if (!fout)
	return -1;
	fwrite(out_data, out_len, 1, fout);

	fclose(fout);
	

	fclose(fin);
	delete[]out_data;
	delete[]second_data;
	delete[]first_data;
	return 0;
}