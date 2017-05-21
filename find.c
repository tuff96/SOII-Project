#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <getopt.h>
#include <stdio.h>


char s[1204];

int info(char *path) {
  char *rpath;
  rpath = realpath(path, NULL);

  if (rpath == NULL) {
    fprintf(stderr, "Error:Nu am gasit fisierul %s\n", path);
    return 0;
  }

  free(rpath);
  rpath = NULL;
  struct stat fStat;
  if (stat(path, &fStat) < 0) {
    fprintf(stderr, "Error:stat error\n");
    return 0;
  }
  fprintf(stdout, "<%ld> ", (long)fStat.st_blocks);        // Numar de inoduri:
  fprintf(stdout, "<%ld> ", (long)fStat.st_mode & S_IFMT); // file type :
  fprintf(stdout, "<%c%c%c%c%c%c%c%c%c%c> ",               // Permisiuni:
          (S_ISDIR(fStat.st_mode)) ? 'd' : '-',

          (fStat.st_mode & S_IRUSR) ? 'r' : '-',
          (fStat.st_mode & S_IWUSR) ? 'w' : '-',
          (fStat.st_mode & S_IXUSR) ? 'x' : '-',

          (fStat.st_mode & S_IRGRP) ? 'r' : '-',
          (fStat.st_mode & S_IWGRP) ? 'w' : '-',
          (fStat.st_mode & S_IXGRP) ? 'x' : '-',

          (fStat.st_mode & S_IROTH) ? 'r' : '-',
          (fStat.st_mode & S_IWOTH) ? 'w' : '-',
          (fStat.st_mode & S_IXOTH) ? 'x' : '-');
  fprintf(stdout, "<%ld> ", fStat.st_nlink);     // Numar de legaturi:
  fprintf(stdout, "<%ld> ", (long)fStat.st_gid); // group ip :
  fprintf(stdout, "<%ld> ", (long)fStat.st_uid); // user id:
  fprintf(stdout, "<%ld> \n", fStat.st_size);    // Dimensiune:
  // fprintf(stdout, "<%ld> \n", fStat.st_birthtimespec); // timp creere

  return 1;
}

int find(const char *path,int depth) {
  struct dirent *entry;
  DIR *dp;
  if (depth < 0)
    return 0;
  dp = opendir(path); // deschide director
  chdir(path);        // seteaza folder actual ca path
  printf("\033[22;34m\n%sFolder: %s\n\033[0m", s, path);
  s[strlen(s)] = '+';
  s[strlen(s) + 1] = 0;
  if (dp == NULL) {    // verifica daca ii null
    perror("opendir"); // eroare
    return -1;
  }

  while ((entry = readdir(dp))) {
    if (strcmp(entry->d_name, ".") == 0 ||
        strcmp(entry->d_name, "..") == 0) // daca ii . sau .. nu le ia
      continue;
    printf("<%s%s> ", s, entry->d_name); // ia numele
    info(entry->d_name);
    if (entry->d_type == DT_DIR) // verifica daca ii folder
    {
      find(entry->d_name,depth - 1);
    }
  }
  closedir(dp); // inchide director
  chdir("..");  // iese din folderul curent
  s[strlen(s) - 1] = 0;
  printf("\e[0;31m%siese din Folder-ul: %s \n\033[0m \n", s, path);
  return 0;
}


int main ( int argc , char *argv[] ) {
  s[0] = 0;
  int c;

if(argc>1)
{
  while (1)
    {
      static struct option long_options[] =
        {
          {"adancime-maxima", required_argument, 0, 'a'},
          {"path",            required_argument, 0, 'p'},
          {"permisiuni,",     required_argument, 0, 'P'},
          {"iname",           required_argument, 0, 'i'},
          {"dim-max",         required_argument, 0, 'M'},
          {"dim-min",         required_argument, 0, 'm'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "a:p:i:M:m:",
                       long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
        {
        case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
          printf ("option %s", long_options[option_index].name);
          if (optarg)
            printf (" with arg %s", optarg);
          printf ("\n");
          break;

        case 'a':
          find(".",optarg);
          break;

        case 'p':
          puts ("option -p\n");
          break;

        case 'P':
          puts ("option -p\n");
          break;

        case 'M':
          printf ("option -M with value `%s'\n", optarg);
          break;

        case 'i':
          printf ("option -i with value `%s'\n", optarg);
          break;

        case 'm':
          printf ("option -m with value `%s'\n", optarg);
          break;

        case '?':
          /* getopt_long already printed an error message. */
          break;

        default:
            printf("?? getopt returned character code 0%o ??\n", c);
        }
    }
 }
 else
  find(".",0);

  return 0;


}
