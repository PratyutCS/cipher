#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <limits.h>

#define Rounds 5

int sBox[16]    = {6, 4, 12, 5, 0, 7, 2, 14, 1, 15, 3, 13, 8, 10, 9, 11};
int permut[16]  = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15};
int invSBox[16] = {4, 8, 6, 10, 1, 3, 0, 5, 12, 14, 13, 15, 2, 11, 7, 9};

// Apply S-box substitution
uint16_t applySBox(uint16_t input) {
    return (sBox[input & 0xF])
         | (sBox[(input >> 4) & 0xF] << 4)
         | (sBox[(input >> 8) & 0xF] << 8)
         | (sBox[(input >> 12) & 0xF] << 12);
}

// Apply inverse S-box
uint16_t applySBoxInv(uint16_t input) {
    return (invSBox[input & 0xF])
         | (invSBox[(input >> 4) & 0xF] << 4)
         | (invSBox[(input >> 8) & 0xF] << 8)
         | (invSBox[(input >> 12) & 0xF] << 12);
}

// Apply permutation
uint16_t applyPermutation(uint16_t input) {
    uint16_t output = 0;
    for (int i = 0; i < 16; i++) {
        if ((input >> permut[i]) & 1)
            output |= (1 << i);
    }
    return output;
}

// Generate round keys from a 16-bit base key
void generateRoundKeys(uint16_t baseKey, uint16_t rkey[Rounds + 1]) {
    uint16_t key = baseKey;
    for (int i = 0; i < Rounds + 1; i++) {
        key = applySBox(key);
        key = applyPermutation(key);
        rkey[i] = key;
    }
}

// Encrypt plaintext using the round keys
uint16_t encrypt(uint16_t msg, uint16_t rkey[Rounds + 1]) {
    uint16_t state = msg;
    for (int i = 0; i < Rounds - 1; i++) {
        state ^= rkey[i];
        state = applySBox(state);
        state = applyPermutation(state);
    }
    state ^= rkey[Rounds - 1];
    state = applySBox(state);
    state ^= rkey[Rounds];
    return state;
}

// Decrypt ciphertext using the round keys
uint16_t decrypt(uint16_t ct, uint16_t rkey[Rounds + 1]) {
    uint16_t state = ct;
    state ^= rkey[Rounds];
    state = applySBoxInv(state);
    state ^= rkey[Rounds - 1];
    for (int i = Rounds - 2; i >= 0; i--) {
        state = applyPermutation(state);
        state = applySBoxInv(state);
        state ^= rkey[i];
    }
    return state;
}

// Check function now prints the key and ciphertext along with the message and decrypted text.
void check(uint16_t pt, uint16_t msg, uint16_t key, uint16_t ct) {
    if (pt != msg) {
        printf("[ERROR] Key: %04x, Msg: %04x -> CT: %04x -> Decrypted: %04x\n", key, msg, ct, pt);
    } else {
        printf("[SUCCESS] Plain text %04x matches message %04x with key %04x\n", pt, msg, key);
    }
}

int main() {
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    // Outer loop: use a 32-bit counter for key to allow a huge range (practically infinite)
    for (uint32_t key = 0; key < UINT32_MAX; key++) {
        uint16_t rkey[Rounds + 1];
        // Use only the lower 16 bits of the key for round key generation.
        generateRoundKeys((uint16_t)key, rkey);

        // Inner loop: iterate over all possible 16-bit messages.
        for (uint32_t msg = 0; msg < 0x10000; msg++) {
            uint16_t ct = encrypt((uint16_t)msg, rkey);
            uint16_t pt = decrypt(ct, rkey);
            check(pt, (uint16_t)msg, (uint16_t)key, ct);
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    printf("[MAIN] executed in time: %f seconds\n",
           (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1.0e9);

    return 0;
}
