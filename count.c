#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char s[1204]; // folosit pentru +
int afis_info(char *path) {
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

int listdir(const char *path, int *fisier, int *folder, int depth) {
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
    printf("%s%s\n", s, entry->d_name); // ia numele
    afis_info(entry->d_name);           // apel afis info
    if (entry->d_type == DT_REG)        // verifica daca ii fisier
      (*fisier)++;
    if (entry->d_type == DT_DIR) // verifica daca ii folder
    {
      (*folder)++;
      listdir(entry->d_name, fisier, folder, depth - 1);
    }
  }
  closedir(dp); // inchide director
  chdir("..");  // iese din folderul curent
  s[strlen(s) - 1] = 0;
  printf("\e[0;31m%siese din Folder-ul: %s \n\033[0m", s, path);
  return 0;
}

int main(int argc, char **argv) {
  printf("<numar inode> <tip fisier> <permisiuni> <numar linkuri> <userid> "
         "<groupid> <dimensiune> <timestamp creare>");
  s[0] = 0;
  int fisier = 0, folder = 0;

  if (argc == 1) // daca nu are parametri
    listdir(".", &fisier, &folder, 0);
  else if (argc == 2) // 1 parametru
    listdir(argv[1], &fisier, &folder, 0);
  else if (argc == 3) // 2 parametri
    listdir(argv[1], &fisier, &folder, atoi(argv[2]));
  else if (argc > 3)
    printf("prea multi parametri");

  printf("fisier %d  folder %d\n", fisier, folder); // print fisier si folder
  return 0;
}
