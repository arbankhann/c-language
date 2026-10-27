#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oqs/oqs.h>

int main(void)
{
    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_768);

    if (kem == NULL) {
        printf("ML-KEM-768 is not available.\n");
        return 1;
    }

    unsigned char *public_key = malloc(kem->length_public_key);
    unsigned char *secret_key = malloc(kem->length_secret_key);
    unsigned char *ciphertext = malloc(kem->length_ciphertext);
    unsigned char *alice_secret = malloc(kem->length_shared_secret);
    unsigned char *bob_secret = malloc(kem->length_shared_secret);

    if (public_key == NULL || secret_key == NULL ||
        ciphertext == NULL || alice_secret == NULL ||
        bob_secret == NULL) {
        printf("Memory allocation failed.\n");
        free(public_key);
        free(secret_key);
        free(ciphertext);
        free(alice_secret);
        free(bob_secret);
        OQS_KEM_free(kem);
        return 1;
    }

    /* Bob generates his public and secret keys. */
    if (OQS_KEM_keypair(kem, public_key, secret_key) != OQS_SUCCESS) {
        printf("Key generation failed.\n");
        goto cleanup;
    }

    printf("Bob generated his key pair.\n");

    /* Alice creates a ciphertext and shared secret. */
    if (OQS_KEM_encaps(kem, ciphertext, alice_secret,
                       public_key) != OQS_SUCCESS) {
        printf("Encapsulation failed.\n");
        goto cleanup;
    }

    printf("Alice generated her shared secret.\n");

    /* Bob recovers the shared secret. */
    if (OQS_KEM_decaps(kem, bob_secret, ciphertext,
                       secret_key) != OQS_SUCCESS) {
        printf("Decapsulation failed.\n");
        goto cleanup;
    }

    printf("Bob recovered his shared secret.\n");

    if (memcmp(alice_secret, bob_secret,
               kem->length_shared_secret) == 0) {
        printf("SUCCESS: Both shared secrets match!\n");
    } else {
        printf("ERROR: Shared secrets do not match.\n");
        goto cleanup;
    }

    free(public_key);
    free(secret_key);
    free(ciphertext);
    free(alice_secret);
    free(bob_secret);
    OQS_KEM_free(kem);
    return 0;

cleanup:
    free(public_key);
    free(secret_key);
    free(ciphertext);
    free(alice_secret);
    free(bob_secret);
    OQS_KEM_free(kem);
    return 1;
}