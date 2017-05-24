#include <dirent.h>
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 4
#endif
#include <time.h>

char s[1204];

int *User_Id = NULL;
int *Group_Id = NULL;
int *Hardlink_Max = NULL;
int *Hardlink_Min = NULL;
char *Iname = NULL;
char *Permisiuni = NULL;
float *Dim_Max = NULL;
float *Dim_Min = NULL;
time_t *Timestamp_Min = NULL;
time_t *Timestamp_Max = NULL;
time_t *Timestamp_Mdata_Max = NULL;
time_t *Timestamp_Mdata_Min = NULL;

time_t timetransform(char *timp) {
  struct tm *tm;
  if (strptime(time, "%a %m/%d/%Y %r", tm) == NULL) {
    return 0;
  }
  return mktime(tm);
}

int verificarenume(char *nume) {
  regex_t regex;
  int reti;

  reti = regcomp(&regex, Iname, 0);
  if (reti) {
    fprintf(stderr, "Could not compile regex\n");
    exit(1);
  }

  /* Execute regular expression */
  reti = regexec(&regex, nume, 0, NULL, 0);
  if (!reti) {
    regfree(&regex);
    return 1;
  } else {
    regfree(&regex);
    return 0;
  }
}

int verificarePermisiuni(struct stat *fStat) {

  if (strlen(Permisiuni) != 9) {
    return 0;
  }

  if (((fStat->st_mode & S_IRUSR) == (Permisiuni[0] == 'r' ? 1 : 0)) &&
      ((fStat->st_mode & S_IWUSR) == (Permisiuni[1] == 'w' ? 1 : 0)) &&
      ((fStat->st_mode & S_IXUSR) == (Permisiuni[2] == 'x' ? 1 : 0))

      && ((fStat->st_mode & S_IRGRP) == (Permisiuni[3] == 'r' ? 1 : 0)) &&
      ((fStat->st_mode & S_IWGRP) == (Permisiuni[4] == 'w' ? 1 : 0)) &&
      ((fStat->st_mode & S_IXGRP) == (Permisiuni[5] == 'x' ? 1 : 0))

      && ((fStat->st_mode & S_IROTH) == (Permisiuni[6] == 'r' ? 1 : 0)) &&
      ((fStat->st_mode & S_IWOTH) == (Permisiuni[7] == 'w' ? 1 : 0)) &&
      ((fStat->st_mode & S_IXOTH) == (Permisiuni[8] == 'x' ? 1 : 0)))
    return 1;
  return 0;
}

int verificare(struct stat *fStat) {

  if (User_Id != NULL) {
    if ((long)fStat->st_uid != *User_Id)
      return 0;
  }

  if (Group_Id != NULL) {
    if ((long)fStat->st_gid != *Group_Id)
      return 0;
  }

  if (Hardlink_Max != NULL) {
    if (fStat->st_nlink > *Hardlink_Max)
      return 0;
  }

  if (Hardlink_Min != NULL) {
    if (fStat->st_nlink < *Hardlink_Min)
      return 0;
  }

  if (Dim_Max != NULL) {
    if (fStat->st_size > *Dim_Max)
      return 0;
  }

  if (Dim_Min != NULL) {
    if (fStat->st_size < *Dim_Min)
      return 0;
  }

  if (Timestamp_Min != NULL) {
    if (fStat->st_ctime < *Timestamp_Min)
      return 0;
  }

  if (Timestamp_Min != NULL) {
    if (fStat->st_ctime > *Timestamp_Max)
      return 0;
  }

  if (Timestamp_Mdata_Min != NULL) {
    if (fStat->st_mtime < *Timestamp_Mdata_Min)
      return 0;
  }

  if (Timestamp_Mdata_Max != NULL) {
    if (fStat->st_mtime > *Timestamp_Mdata_Max)
      return 0;
  }

  if (Permisiuni != NULL) {
    if (verificarePermisiuni(fStat) == 0)
      return 0;
  }

  return 1;
}

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
  if (verificare(&fstat) == 0)
    return 0;
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

int find(const char *path, int depth) {
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
    if (Iname != NULL) {
      if (verificarenume(entry->d_name) == 0)
        continue;
    }
    printf("<%s%s> ", s, entry->d_name); // ia numele
    info(entry->d_name);
    if (entry->d_type == DT_DIR) // verifica daca ii folder
    {
      find(entry->d_name, depth - 1);
    }
  }
  closedir(dp); // inchide director
  chdir("..");  // iese din folderul curent
  s[strlen(s) - 1] = 0;
  printf("\e[0;31m%siese din Folder-ul: %s \n\033[0m \n", s, path);
  return 0;
}

int main(int argc, char *argv[]) {
  s[0] = 0;
  int c;
  int Adancime = 0;
  char *Path = "./";

  while (1) {
    static struct option long_options[] = {
        {"adancime-maxima", required_argument, 0, 'a'},
        {"path", required_argument, 0, 'p'},
        {"permisiuni,", required_argument, 0, 'P'},
        {"iname", required_argument, 0, 'i'},
        {"dim-max", required_argument, 0, 'D'},
        {"dim-min", required_argument, 0, 'd'},
        {"userid", required_argument, 0, 'u'},
        {"groupid", required_argument, 0, 'g'},
        {"hardlink-min", required_argument, 0, 'h'},
        {"hardlink-max", required_argument, 0, 'H'},
        {"timestamp-min", required_argument, 0, 't'},
        {"timestamp-max", required_argument, 0, 'T'},
        {"timestamp-mdata-min", required_argument, 0, 'm'},
        {"timestamp-mdata-max", required_argument, 0, 'M'},
        {0, 0, 0, 0}};
    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long(argc, argv, "a:p:P:i:D:d:u:g:h:H:t:T:m:M:", long_options,
                    &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c) {
    case 0:
      /* If this option set a flag, do nothing else now. */
      if (long_options[option_index].flag != 0)
        break;
      printf("option %s", long_options[option_index].name);
      if (optarg)
        printf(" with arg %s", optarg);
      printf("\n");
      break;

    case 'a':
      Adancime = atoi(optarg);
      break;

    case 'u':
      *User_Id = atoi(optarg);
      break;

    case 'p':
      Path = optarg;
      break;

    case 'P':
      Permisiuni = optarg;
      break;

    case 'M':
      *Timestamp_Mdata_Max = timetransform(optarg);
      break;

    case 'm':
      *Timestamp_Mdata_Min = timetransform(optarg);
      break;

    case 'i':
      Iname = optarg;
      break;

    case 'g':
      Group_Id = atoi(optarg);
      break;

    case 'h':
      *Hardlink_Min = atoi(optarg);
      break;

    case 'H':
      *Hardlink_Max = atoi(optarg);
      break;

    case 't':
      *Timestamp_Min = timetransform(optarg);
      break;

    case 'T':
      *Timestamp_Max = timetransform(optarg);
      break;

    case 'd':
      *Dim_Min = atol(optarg);
      break;

    case 'D':
      *Dim_Max = atol(optarg);
      break;

    case '?':
      /* getopt_long already printed an error message. */
      break;

    default:
      printf("?? getopt returned character code 0%o ??\n", c);
    }
  }
  find(Path, Adancime);

  return 0;
}
