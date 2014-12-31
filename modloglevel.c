#include <stdio.h>
#include <ldap.h>
#include <math.h>

int main(int argc, char* argv[])
{
  /* 引数を判定 */
  int i;
  float n;
  if (argc < 2) {
    fprintf(stderr, "USAGE: %s integer\n", argv[0]);
    return 1;
  } else {
    for (i = 1; i < argc; i++) {
      n = atoi(argv[i]);
      if (n > 32768) {
        fprintf(stderr, "n < 2049\n");
        return 1;
      }
      if ((n == 1) || (n == -1) || (n == 16384) || (n == 32768)) {
        break;
      }
      while (1) {
        n = n / 2;
        if (n == 1) {
          break;
        }
        if (n < 1) {
          fprintf(stderr, "n is power of 2 or -1.\n");
          return 1;
        }
      }
    }
  }

  /* loglevel を計算 */
  int sum;
  char level[6];

  sum = 0;
  for (i = 1; i < argc; i++) {
    n = atoi(argv[i]);
    sum = sum + (int)n;
  }
  sprintf(level, "%d", sum);

  printf("loglevel is %s\n", level); 

  /* セッションハンドルの取得 */
  LDAP *ld;

  ld = ldap_init("localhost", LDAP_PORT);

  if (ld == NULL) {
    perror("ldap_init");
    return 1;
  }

  char *url = "ldapi:///";
  ldap_set_option(ld, LDAP_OPT_URI, url);

  int version;
  version = LDAP_VERSION3;
  ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &version);


  /* ユーザ DN cn=config でバインド */
  int statusb;
  statusb = ldap_simple_bind_s(ld, "cn=config", "secret");
  if (statusb != LDAP_SUCCESS) {
    ldap_perror(ld, "ldap_simple_bind_s");
    return 1;
  }

  /* loglevelの変更 */
  LDAPMod mod;
  LDAPMod *mods[2];
  char *vals[2];

  /* 更新データ */
  vals[0] = level;
  vals[1] = NULL;
  mod.mod_op = LDAP_MOD_REPLACE;
  mod.mod_type = "olcLogLevel";
  mod.mod_values = vals;
  mods[0] = &mod;
  mods[1] = NULL;

  /* 更新操作 */
  int statusm;
  statusm = ldap_modify_ext_s(ld, "cn=config", mods, NULL, NULL);
  if (statusm != LDAP_SUCCESS) {
    ldap_perror(ld, "ldap_modify_ext_s");
    return 1;
  }

  /* アンバインド */
  int c;
  c = ldap_unbind_ext_s(ld, NULL, NULL);
  if (c != 0) {
    ldap_perror(ld, "ldap_unbind_ext_s");
    return 1;
  }
  return 0;
}





