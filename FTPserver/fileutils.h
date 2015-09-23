
#ifndef _FILEUTILS_H
#define _FILEUTILS_H

#ifdef __cplusplus
extern "C"
{
#endif
	bool write_list(int , int , const char *);
	bool stat_file(int, const char *,char *);
	bool change_dir(int ,const char *,char *,char *,char *);
	bool retrieve_file(int, int, int, const char *);
	bool store_file(int, int, int, const char *);
	bool stou_file(int , int, int, int);
	bool make_dir(int,const char*,char *);
	bool remove_dir(int ,const char *);
	bool delete_file(int ,const char *);
	bool rename_fr(int ,const char *,const char *);
#ifdef __cplusplus
}
#endif

#endif /* _FILEUTILS_H */
