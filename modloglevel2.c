#include <stdio.h>
#include <ldap.h>
#include <math.h>

int checkArgs(int[] args)
{
  int i;
  float n;
  int length = sizeof args / args[0];
  for (i = 0; i < length; i++) {
    n = atoi(args[i]);
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

int sumArgs(int[] args) {
  int sum = 0;
  int i;
  int n;
  int length = sizeof args / args[0];
  for (i = 0; i < length; i++) {
    n = atoi(args[i]);
    sum = sum + (int)n;
  }
}

int ldapConnection()
{
  LDAP *ld;
  ld = ldap_init("localhost", LDAP_PORT);
  if (ld == NULL) {
    return 1;
  }

  char *url = "ldapi:///";
  ldap_set_option(ld, LDAP_OPT_URI, url);

  int version = LDAP_VERSION3;
  ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &version);

  return 0;
}


int main(int argc, char* argv[])
{
  int i;
  float n;
  /* 引数を判定 */
  if (argc < 2) {
    fprintf(stderr, "使い方: %s n m ...\n n, m,... は -1 か 2 のべき乗で 32768以下", argv[0]);
  }
  if (checkArgs(argv)) {
    fprintf(stderr, "使い方: %s n m ...\n n, m,... は -1 か 2 のべき乗で 32768以下", argv[0]);
  }

  /* loglevel を計算 */
  char level[6];
  sprintf(level, "%d", sumArgs(argv));


  printf("loglevel is %s\n", level);

  /* セッションハンドルの取得 */
  if (ldapConnection()) {
    perror("ldap_init");
  }

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





