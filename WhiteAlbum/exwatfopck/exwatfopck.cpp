// exwatfopck.cpp, v1.0 2010/10/14
// coded by asmodean

// contact: 
//   web:   http://asmodean.reverse.net
//   email: asmodean [at] hush.com
//   irc:   asmodean on efnet (irc.efnet.net)

//	 modified by 福音(Fuyin)
//   2014/5/11

// This tool extracts and rebuild *.pck archives used by WHITE ALBUM亅捲傜傟傞搤偺憐偄弌亅 (PS3).

#include "as-util.h"

struct SECTHDR {
  unsigned char signature[8]; // "Filename"
  unsigned long length;
};

struct PARTHDR {
  unsigned char signature[8]; // "Pack    "
  unsigned long index_size;
  unsigned long part_count;
};

struct PARTENTRY {
  //每个part在游戏屏幕中的长宽
  float screen_width;
  float screen_height;
  //每个part在整张图片中左上角坐标的因数，乘以整张图的长或宽为实际坐标
  float pic_mult_x1;
  float pic_mult_y1;
  //每个part在整张图片中右下角坐标的因数，乘以整张图的长或宽为实际坐标
  float pic_mult_x2;
  float pic_mult_y2;
};

struct FNENTRY {
  unsigned long offset;
};

struct TOCHDR {
  unsigned long entry_count;
};

struct TOCENTRY {  
  unsigned long offset;
  unsigned long length;
};

struct TEXHDR {
  unsigned long unknown1;
  unsigned long data_length;
  unsigned long width;
  unsigned long height;
  unsigned long width2;
  unsigned long height2;
};

int main(int argc, char** argv)
{

	bool do_rebuild = false;

	if (argc < 2)
	{
		fprintf(stderr, "exwatfopck v1.1 by asmodean & modified by 福音(Fuyin)\n\n");
		fprintf(stderr, "usage: %s <input.pck> [output.pck]\n", argv[0]);
		return -1;
	}

	if (argc == 3)
		do_rebuild = true;


	string in_filename(argv[1]);


	int fd = as::open_or_die(in_filename, O_RDONLY | O_BINARY);

	unsigned long index_block_size = 0;

	int out_fd = -1;
	if (do_rebuild)
	{
		string out_filename(argv[2]);
		out_fd = as::open_or_die(out_filename, O_CREAT | O_TRUNC | O_WRONLY | O_BINARY);
	}

	SECTHDR secthdr;
	read(fd, &secthdr, sizeof(secthdr));
	if (do_rebuild)
		write(out_fd, &secthdr, sizeof(secthdr));

	unsigned long  fn_len = ((as::flip_endian(secthdr.length) - sizeof(secthdr)) + 3 & ~3);
	unsigned char* fn_buff = new unsigned char[fn_len];
	read(fd, fn_buff, fn_len);
	if (do_rebuild)
		write(out_fd, fn_buff, fn_len);

	FNENTRY* fn_entries = (FNENTRY*)fn_buff;

	if (do_rebuild)
		index_block_size += as::flip_endian(secthdr.length) + 3 & ~3;

	read(fd, &secthdr, sizeof(secthdr));
	if (do_rebuild)
		write(out_fd, &secthdr, sizeof(secthdr));

	unsigned long  toc_len = as::flip_endian(secthdr.length) - sizeof(secthdr);
	unsigned char* toc_buff = new unsigned char[toc_len];
	read(fd, toc_buff, toc_len);


	TOCHDR*   toc_hdr = (TOCHDR*)toc_buff;
	if (do_rebuild)
		write(out_fd, toc_hdr, sizeof(TOCHDR));

	TOCENTRY* toc_entries = (TOCENTRY*)(toc_hdr + 1);

	if (do_rebuild)
		index_block_size += as::flip_endian(secthdr.length);

	unsigned long entry_count = as::flip_endian(toc_hdr->entry_count);

	TOCENTRY* out_toc_entries = NULL;
	if (do_rebuild)
	{
		lseek(out_fd, index_block_size, SEEK_SET);
		out_toc_entries = new TOCENTRY[entry_count];
	}
		

	for (unsigned long i = 0; i < entry_count; i++)
	{
		char* filename = (char*)(fn_buff + as::flip_endian(fn_entries[i].offset));

		if (do_rebuild)
		{
			int cur_fd = as::open_or_die(filename ,O_RDONLY | O_BINARY);
			if (cur_fd == -1)
			{
				fprintf(stderr, "can not found file %s\n", filename);
				return -1;
			}
			unsigned long offset = tell(out_fd) ;
			unsigned long real_offset = offset + 15 & ~15;
			out_toc_entries[i].offset = as::flip_endian(real_offset);

			unsigned long len = as::get_file_size(cur_fd);
			out_toc_entries[i].length = as::flip_endian(len);

			unsigned char *buff = new unsigned char[len];
			read(cur_fd, buff, len);

			unsigned long zeros = real_offset - offset;
			unsigned char *z_buff = new unsigned char[zeros];
			memset(z_buff, 0, zeros);
			write(out_fd, z_buff, zeros);

			write(out_fd, buff, len);

			delete[] z_buff;
			delete[] buff;
			close(cur_fd);
		}
		else
		{
			unsigned long  offset = as::flip_endian(toc_entries[i].offset);
			unsigned long  len = as::flip_endian(toc_entries[i].length);
			unsigned char* buff = new unsigned char[len];
			lseek(fd, offset, SEEK_SET);
			read(fd, buff, len);

			/*
			TEXHDR* texhdr = NULL;

			if (len >= 8 && !memcmp(buff, "Texture ", 8))
			{
				texhdr = (TEXHDR*)(buff + sizeof(SECTHDR));
			}
			else if (len > sizeof(TEXHDR) && as::stringtol(filename).find(".tex") != string::npos)
			{
				texhdr = (TEXHDR*)buff;
			}

			// Other types are MotionPortrait data... we could extract the PNG texture but it's useless
			// garbage until I reverse the MP format.
			if (texhdr && as::flip_endian(texhdr->unknown1) == 1)
			{
				unsigned long  data_len = as::flip_endian(texhdr->data_length);
				unsigned char* data_buff = (unsigned char*)(texhdr + 1);

				as::write_file(as::get_file_prefix(filename) + as::guess_file_extension(data_buff, data_len), data_buff, data_len);
			}
			else
			{
				as::write_file(filename, buff, len);
			}
			*/

			as::write_file(filename, buff, len);
			delete[] buff;
		}
	}

	if (do_rebuild)
	{
		lseek(out_fd, index_block_size - entry_count*sizeof(TOCENTRY), SEEK_SET);
		write(out_fd, out_toc_entries, entry_count*sizeof(TOCENTRY));

		delete[] out_toc_entries;
		close(out_fd);
	}

	delete[] toc_buff;
	delete[] fn_entries;

	close(fd);

	return 0;
}
