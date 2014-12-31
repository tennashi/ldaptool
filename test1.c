#include <stdio.h>
#include <ldap.h>

int main(void)
{
  LDAP* ld;
  int version;
  int status;

  LDAPMessage *result, *e;
  BerElement *ber;
  char *a, *dn;
  char **vals;
  int i;

  ld =ldap_init("localhost", LDAP_PORT);
  if (ld == NULL) {
    perror("ldap_init");
    return 1;
  }

  version = LDAP_VERSION3;
  ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &version);

  status = ldap_search_ext_s(ld, "dc=math,dc=kyoto-u,dc=ac,dc=jp", LDAP_SCOPE_SUBTREE, "(cn=yuya.gt)", NULL, 0, NULL, NULL, NULL, 0, &result);

  if (status != LDAP_SUCCESS) {
    ldap_perror(ld, "ldap_search_ext_s");
    return 1;
  }

  for (e = ldap_first_entry(ld, result); e != NULL; e = ldap_next_entry(ld, e)) {
    dn = ldap_get_dn(ld, e);
    if (dn != NULL) {
      printf("dn: %s\n", dn);
      ldap_memfree(dn);
    }

    for (a = ldap_first_attribute(ld, e, &ber); a != NULL; a = ldap_next_attribute(ld, e, ber)) {
      vals = ldap_get_values(ld, e, a);

      if (vals != NULL) {
        for (i = 0; vals[i] != NULL; i++) {
          printf("%s: %s\n", a, vals[i]);
        }
        ldap_value_free(vals);
      }
      ldap_memfree(a);
    }
    if (ber != NULL) {
      ber_free(ber, 0);
    }
    printf("\n");
  }
  ldap_msgfree(result);

  ldap_unbind_ext(ld,NULL,NULL);
  return 0;
}
