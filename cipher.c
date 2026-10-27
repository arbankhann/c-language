
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#define KEY_LEN 32
#define IV_LEN 12
#define TAG_LEN 16

int main(void)
{
    // 1. Secret key: 32 bytes = 256 bits
    unsigned char key[KEY_LEN];

    // 2. Unique nonce for this encryption
    unsigned char iv[IV_LEN];

    // 3. Original message
    unsigned char plaintext[] = "Hello World";

    // 4. Output buffers
    unsigned char ciphertext[128];
    unsigned char decrypted[128];
    unsigned char tag[TAG_LEN];

    int len = 0;
    int ciphertext_len = 0;
    int decrypted_len = 0;
    int final_len = 0;

    EVP_CIPHER_CTX *ctx = NULL;
    int success = 0;

    // Generate a cryptographically secure key and nonce
    if (RAND_bytes(key, sizeof(key)) != 1 ||
        RAND_bytes(iv, sizeof(iv)) != 1) {
        fprintf(stderr, "Random generation failed\n");
        goto cleanup;
    }

    // Create the cipher context
    ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        fprintf(stderr, "Context allocation failed\n");
        goto cleanup;
    }

    // 5. Initialize AES-256-GCM encryption
    if (EVP_EncryptInit_ex(
            ctx, EVP_aes_256_gcm(), NULL, key, iv) != 1) {
        fprintf(stderr, "Encryption initialization failed\n");
        goto cleanup;
    }

    // 6. Encrypt plaintext into ciphertext
    if (EVP_EncryptUpdate(
            ctx, ciphertext, &len,
            plaintext, (int)strlen((char *)plaintext)) != 1) {
        fprintf(stderr, "Encryption failed\n");
        goto cleanup;
    }

    ciphertext_len = len;

    // 7. Finish encryption
    if (EVP_EncryptFinal_ex(
            ctx, ciphertext + ciphertext_len, &len) != 1) {
        fprintf(stderr, "Encryption finalization failed\n");
        goto cleanup;
    }

    ciphertext_len += len;

    // 8. Obtain the authentication tag
    if (EVP_CIPHER_CTX_ctrl(
            ctx, EVP_CTRL_AEAD_GET_TAG, TAG_LEN, tag) != 1) {
        fprintf(stderr, "Tag generation failed\n");
        goto cleanup;
    }

    printf("Original: %s\n", plaintext);
    printf("Ciphertext: ");

    for (int i = 0; i < ciphertext_len; i++) {
        printf("%02x", ciphertext[i]);
    }
    printf("\n");

    // Reinitialize context for decryption
    if (EVP_DecryptInit_ex(
            ctx, EVP_aes_256_gcm(), NULL, key, iv) != 1) {
        fprintf(stderr, "Decryption initialization failed\n");
        goto cleanup;
    }

    // 9. Decrypt ciphertext
    if (EVP_DecryptUpdate(
            ctx, decrypted, &len,
            ciphertext, ciphertext_len) != 1) {
        fprintf(stderr, "Decryption failed\n");
        goto cleanup;
    }

    decrypted_len = len;

    // 10. Tell OpenSSL which tag must be verified
    if (EVP_CIPHER_CTX_ctrl(
            ctx, EVP_CTRL_AEAD_SET_TAG, TAG_LEN, tag) != 1) {
        fprintf(stderr, "Tag setup failed\n");
        goto cleanup;
    }

    // 11. Finalize: authentication must succeed
    if (EVP_DecryptFinal_ex(
            ctx, decrypted + decrypted_len, &final_len) != 1) {
        fprintf(stderr, "Authentication failed!\n");
        goto cleanup;
    }

    decrypted_len += final_len;
    decrypted[decrypted_len] = '\0';

    printf("Decrypted: %s\n", decrypted);
    success = 1;

cleanup:
    EVP_CIPHER_CTX_free(ctx);

    // Clear secret key from this buffer
    OPENSSL_cleanse(key, sizeof(key));

    return success ? 0 : 1;
}
