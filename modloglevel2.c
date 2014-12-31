#include <stdio.h>
#include <ldap.h>
#include <math.h>

int checkArgs(int* args)
{
  int i;
  float n;
  int length = sizeof args / args[0];
  for (i = 0; i < length; i++) {
    n = atof(args[i]);
    if (n > 32768) {
      return 1;
    }
    if (n == -1) {
      break;
    }
    while (1) {
      if (n == 1) {
        break;
      }
      n = n / 2;
      if (n < 1) {
        return 1;
      }
    }
  }
  return 0;
}

int sumArgs(int* args) {
  int sum = 0;
  int i;
  int n;
  int length = sizeof args / args[0];
  for (i = 0; i < length; i++) {
    n = atoi(args[i]);
    sum = sum + (int)n;
  }
}

LDAP *ldapConnection()
{
  LDAP *ld;
  ld = ldap_init("localhost", LDAP_PORT);
  if (ld == NULL) {
    return ld;
  }

  char *url = "ldapi:///";
  ldap_set_option(ld, LDAP_OPT_URI, url);

  int version = LDAP_VERSION3;
  ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &version);

  return ld;
}

int ldapBind(LDAP *ld)
{
  int status;
  status = ldap_simple_bind_s(ld, "cn=config", "secret");
  if (status != LDAP_SUCCESS) {
    return 1;
  }
  return 0;
}

int ldapModLoglevel(LDAP *ld, char level)
{
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

  int status;
  status = ldap_modify_ext_s(ld, "cn=config", mods, NULL, NULL);
  if (status != LDAP_SUCCESS) {
    return 1;
  }
  return 0;
}

int ldapUnbind(LDAP *ld)
{
  int status;
  status = ldap_unbind_ext_s(ld, NULL, NULL);
  if (status != LDAP_SUCCESS) {
    return 1;
  }
  return 0;
}


int main(int argc, char* argv[])
{
  /* 引数を判定 */
  if (argc < 2) {
    fprintf(stderr, "使い方: %s n m ...\n n, m,... は -1 か 2 のべき乗で 32768以下\n", argv[0]);
    return 1;
  }
  if (checkArgs(argv)) {
    fprintf(stderr, "使い方: %s n m ...\n n, m,... は -1 か 2 のべき乗で 32768以下\n", argv[0]);
    return 1;
  }

  /* loglevel を計算 */
  char level[6];
  sprintf(level, "%d", sumArgs(argv));


  printf("loglevel is %s\n", level);

  /* セッションハンドルの取得 */
  LDAP *ld = ldapConnection();
  if (ld == NULL) {
    perror("ldap_init");
    return 1;
  }

  /* ユーザ DN cn=config でバインド */
  if (ldapBind(ld)) {
    ldap_perror(ld, "ldap_simple_bind_s");
    return 1;
  }

  /* loglevelの変更 */
  if (ldapModLoglevel(ld, level)) {
    ldap_perror(ld, "ldap_modify_ext_s");
    return 1;
  }

  /* アンバインド */
  if (ldapUnbind(ld)) {
    ldap_perror(ld, "ldap_unbind_ext_s");
    return 1;
  }
  return 0;
}





