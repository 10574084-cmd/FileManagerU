#include "file_ops.hpp"
#include <algorithm>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
std::string joinPath(const std::string&a,const std::string&b){return a.empty()?b:(a.back()=='/'?a+b:a+"/"+b);}
bool listDirectory(const std::string&p,std::vector<FileEntry>&v,std::string&e){v.clear();DIR*d=opendir(p.c_str());if(!d){e=std::string("Cannot open: ")+strerror(errno);return false;}while(dirent*x=readdir(d)){std::string n=x->d_name;if(n=="."||n=="..")continue;struct stat s{};if(stat(joinPath(p,n).c_str(),&s))continue;v.push_back({n,S_ISDIR(s.st_mode),S_ISDIR(s.st_mode)?0ULL:(unsigned long long)s.st_size});}closedir(d);std::sort(v.begin(),v.end(),[](auto&a,auto&b){return a.directory!=b.directory?a.directory>b.directory:a.name<b.name;});return true;}
bool copyFile(const std::string&a,const std::string&b,std::string&e){struct stat s{};if(stat(a.c_str(),&s)||!S_ISREG(s.st_mode)){e="Only regular-file copy is supported";return false;}FILE*i=fopen(a.c_str(),"rb");if(!i){e=strerror(errno);return false;}FILE*o=fopen(b.c_str(),"wb");if(!o){e=strerror(errno);fclose(i);return false;}char buf[32768];bool ok=true;for(;;){size_t n=fread(buf,1,sizeof(buf),i);if(n&&fwrite(buf,1,n,o)!=n){ok=false;break;}if(n<sizeof(buf)){if(ferror(i))ok=false;break;}}fclose(i);fclose(o);if(!ok){remove(b.c_str());e="Copy failed";}return ok;}
bool moveFile(const std::string&a,const std::string&b,std::string&e){if(rename(a.c_str(),b.c_str())==0)return true;if(!copyFile(a,b,e))return false;if(remove(a.c_str())){e="Copied, source removal failed";return false;}return true;}
bool renamePath(const std::string&a,const std::string&b,std::string&e){if(rename(a.c_str(),b.c_str())==0)return true;e=strerror(errno);return false;}
bool deleteFile(const std::string&p,std::string&e){struct stat s{};if(lstat(p.c_str(),&s)){e=strerror(errno);return false;}if(S_ISDIR(s.st_mode)){e="Directory deletion disabled in this build";return false;}if(remove(p.c_str())){e=strerror(errno);return false;}return true;}
