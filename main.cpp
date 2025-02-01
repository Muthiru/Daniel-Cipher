
#include <iostream>
#include <cstdint>
#include <bitset>
using namespace std;

class DanielCipher {
private:

    static const int ROUNDS = 4;
    uint64_t masterKey;
    uint16_t roundKeys[4];

    // Substitution box (S-box) for non-linear transformation
    const uint8_t SBOX[16] = {
            0xE, 0x4, 0xD, 0x1,
            0x2, 0xF, 0xB, 0x8,
            0x3, 0xA, 0x6, 0xC,
            0x5, 0x9, 0x0, 0x7
    };
    // Permutation box (P-box) for bit position rearrangement
    const uint8_t PBOX[16] = {
            1, 5, 9, 13,
            2, 6, 10, 14,
            3, 7, 11, 15,
            4, 8, 12, 16
    };


        void generateRoundKeys() {  // Generate round keys from master key
            // Split 64-bit master key into two 32-bit halves
            uint32_t leftHalf = masterKey >> 32;
            uint32_t rightHalf = masterKey & 0xFFFFFFFF;
            //  output for key generation process
            cout << "\nRound Key Generation:" << endl;
            cout << "Left Half: 0x" << hex << leftHalf << endl;
            cout << "Right Half: 0x" << hex << rightHalf << endl;


            for(int i = 0; i < ROUNDS; i++) {    // Generate 4 different round keys
            uint32_t temp;
            if(i < 2) {
                // First two round keys use left half with rotations of 3 and 5
                temp = rotateLeft(leftHalf, 3 + 2*i);
            } else {
                // Last two round keys use right half with rotations of 7 and 9
                temp = rotateLeft(rightHalf, 7 + 2*(i-2));
            }
            // XOR with round number and mask to 16 bits
            roundKeys[i] = (temp ^ (i + 1)) & 0xFFFF;
            cout << "Round Key " << i + 1 << ": 0x" << hex << roundKeys[i] << endl;
        }
    }

    // Rotate bits left by specified amount
    uint32_t rotateLeft(uint32_t value, int shift) {
        return ((value << shift) | (value >> (32 - shift))) & 0xFFFFFFFF;
    }

    // F-function: core transformation function
    uint16_t fFunction(uint16_t input, uint16_t roundKey) {
        // Apply S-box substitution
        uint16_t result = 0;
        for(int i = 0; i < 4; i++) {
            // Extract 4-bit chunks and substitute using S-box
            uint8_t chunk = (input >> (4 * i)) & 0xF;
            result |= ((uint16_t)SBOX[chunk] << (4 * i));
        }

        // Apply permutation
        uint16_t permuted = 0;
        for(int i = 0; i < 16; i++) {

            int newPos = PBOX[i] - 1;
            if(result & (1 << i)) {    // Rearrange bits according to P-box
                permuted |= (1 << newPos);
            }
        }

        // Add round key and return result
        return (permuted + roundKey) & 0xFFFF;
    }

public:
    // Constructor: initialize cipher with master key
    DanielCipher(uint64_t key) : masterKey(key) {
        generateRoundKeys();
    }

    // Encryption function
    uint32_t encrypt(uint32_t plaintext) {
        // Split 32-bit plaintext into two 16-bit halves
        uint16_t left = plaintext >> 16;
        uint16_t right = plaintext & 0xFFFF;


        cout << "\nEncryption process:" << endl;  //  output
        cout << "Initial: L=" << hex << left << " R=" << right << endl;

        // Apply  rounds
        for(int i = 0; i < ROUNDS; i++) {
            uint16_t temp = right;
            right = left ^ fFunction(right, roundKeys[i]);
            left = temp;

            cout << "Round " << i + 1 << ": L=" << hex << left << " R=" << right << endl;
        }

        // Combine final blocks and return ciphertext
        return ((uint32_t)left << 16) | right;
    }

    // Decryption function
    uint32_t decrypt(uint32_t ciphertext) {
        // Split 32-bit ciphertext into two 16-bit halves
        uint16_t left = ciphertext >> 16;
        uint16_t right = ciphertext & 0xFFFF;


        cout << "\nDecryption process:" << endl;  //  output
        cout << "Initial: L=" << hex << left << " R=" << right << endl;

        // Apply  rounds in reverse
        for(int i = ROUNDS - 1; i >= 0; i--) {
            uint16_t temp = left;
            left = right ^ fFunction(left, roundKeys[i]);
            right = temp;

            cout << "Round " << ROUNDS - i << ": L=" << hex << left << " R=" << right << endl;
        }

        // Combine final blocks and return plaintext
        return ((uint32_t)left << 16) | right;
    }
};

// Test function to demonstrate encryption/decryption
void testCipher() {
    // Test values
    uint64_t key = 0xAB12CD34EF567890;    // 64-bit key
    uint32_t plaintext = 0xF7B3E9D1;       // 32-bit plaintext

    // Create cipher instance
    DanielCipher cipher(key);

    // Display initial values
    cout << "Master Key: 0x" << hex << key << endl;
    cout << "Plaintext: 0x" << hex << plaintext << endl;

    // Perform encryption and decryption
    uint32_t ciphertext = cipher.encrypt(plaintext);
    cout << "Ciphertext: 0x" << hex << ciphertext << endl;

    uint32_t decrypted = cipher.decrypt(ciphertext);
    cout << "Decrypted: 0x" << hex << decrypted << endl;

    // Verify results
    cout << "\nVerification: " << (plaintext == decrypted ? "Successful!" : "Failed!") << endl;
}

// Main function
int main() {
    testCipher();    // Run the test
    return 0;
}