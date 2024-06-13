#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <string>
#include <algorithm>
#include <bitset>
#include <stdexcept>

using namespace std;

vector<uint8_t> getBitByteArray(const string& input) {
    vector<uint8_t> byteList;

    for (int i = input.length() - 1; i >= 0; i -= 8) {
        string byteString = "";
        for (int j = 0; (i - j) >= 0 && j < 8; j++) {
            if (input[i - j] == '0' || input[i - j] == '1') {
                byteString = input[i - j] + byteString;
            }
            else {
                throw invalid_argument("Invalid bit input. Only '0' and '1' characters are allowed.");
            }
        }
        if (!byteString.empty()) {
            byteList.push_back(static_cast<uint8_t>(stoi(byteString, nullptr, 2)));
        }
    }

    reverse(byteList.begin(), byteList.end());
    return byteList;
}

// Function to calculate CRC using the specified polynomial (0x4599)
uint16_t calculateCRC(const vector<uint8_t>& data) {
    uint16_t crc = 0;
    const uint16_t polynomial = 0x4599;

    for (uint8_t current : data) {
        crc ^= (uint16_t)(current << 7);
        for (int i = 0; i < 8; i++) {
            crc <<= 1;
            if ((crc & 0x8000) != 0) {
                crc ^= polynomial;
            }
        }
        crc &= 0x7FFF;
    }

    return crc;
}

int main() {
    while (true) {
        vector<uint8_t> byteSequence;
        string bitInput;
        int n;

        while (true) {
            cout << "Wprowadz dane (bitowe, tylko 0 i 1, max 96 bits): ";
            getline(cin, bitInput);

            try {
                if (bitInput.size() > 96) {
                    throw length_error("Bledne dane (max. 96 bits).");
                }
                byteSequence = getBitByteArray(bitInput);
                cout << "Wprowadzone dane bitowe: " << bitInput << endl;
                cout << "Wprowadzone dane w postaci bajtowej (hex): ";
                for (const auto& byte : byteSequence) {
                    cout << hex << uppercase << setw(2) << setfill('0') << static_cast<int>(byte);
                }
                cout << endl;

                // Print each byte in binary format
                cout << "Dane w postaci binarnej:" << endl;
                for (size_t i = 0; i < byteSequence.size(); ++i) {
                    cout << "byte " << i + 1 << " " << bitset<8>(byteSequence[i]) << endl;
                }

                break;
            }
            catch (const invalid_argument& e) {
                cerr << "Bledne dane. " << e.what() << " Wprowadz ponownie" << endl;
            }
            catch (const length_error&) {
                cerr << "Bledne dane (max. 96 bits)." << endl;
            }
        }

        while (true) {
            cout << "Wprowadz liczbe powtorzen obliczania CRC (od 1 do 1000000000): ";
            cin >> n;

            if (cin.fail() || n < 1 || n > 1000000000) {
                cerr << "Bledne dane. Wprowadz liczbe powtorzen (od 1 do 1000000000):" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            else {
                break;
            }
        }

        uint16_t crcValue;
        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < n; ++i) {
            crcValue = calculateCRC(byteSequence);
        }
        auto end = chrono::high_resolution_clock::now();

        chrono::duration<long double, micro> duration = end - start;
        long double duration_ms = duration.count() / 1000.0;
        long double avg_time_per_repetition = duration_ms / n;

        cout << "Obliczone CRC: " << hex << uppercase << setw(4) << setfill('0') << crcValue << endl;
        cout << "Calkowity czas dla " << dec << n << " powtorzen: " << fixed << setprecision(4) << duration_ms << " ms" << endl;
        cout << "Sredni czas na powtorzenie: " << fixed << setprecision(10) << avg_time_per_repetition << " ms" << endl;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    return 0;
}