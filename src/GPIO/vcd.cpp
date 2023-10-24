#include <fstream>
#include <ctime>
#include <iomanip>
#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <bitset>

typedef struct {
    uint8_t size;
    uint32_t mask;
    const char identifider;
    const char* name;
} VCDSignal;

class VCDWriter{
    private:
        std::ofstream outfile;
        char n_signal; //  Between 33 and 71?
        std::unordered_map<VCDSignal*, int> state; // Vector of all signals

    public:
        VCDWriter(const char* filename){
            this->outfile.open(filename, std::ios::out);
            this->n_signal = 0;
        }

        ~VCDWriter(){
            this->outfile.close();
        }

        void write_header(const char* timescale){
            const auto t = std::time(nullptr);
            const auto tm = std::localtime(&t);

            this->outfile << "$date" << std::endl;
            this->outfile << "    " << std::put_time(tm, "%d %m %Y") << std::endl;
            this->outfile << "$end" << std::endl;
            this->outfile << "$version" << std::endl;
            this->outfile << "    ZRarray PIO, RP2040 PIO emulator" << std::endl;
            this->outfile << "$end" << std::endl;
            this->outfile << "$timescale " << timescale << " $end" << std::endl;
            this->outfile << "$scope module logic $end" << std::endl;

            for(auto [pSig, _] : this->state){
                this->outfile << "$var wire " << std::to_string(pSig->size) << " " << pSig->identifider << " " << pSig->name << " $end" << std::endl;
            }

            this->outfile << "$upscope $end" << std::endl;
            this->outfile << "$enddefinitions $end" << std::endl;
            this->outfile << "#0" << std::endl;

            for(auto [sig, _ ]: this->state){
                this->write_value(sig, 0);
            }
        }

        void write_time(const char* timestamp){
            this->outfile << "#" << timestamp << std::endl;
        }

        void write_value(VCDSignal* sig, const uint32_t val){

            uint32_t real_val = val & sig->mask;
            if(this->state[sig] != real_val){
                if(sig->size <= 1){
                    //Use simplified for 1 bit value
                    this->outfile << real_val << sig->identifider << std::endl;
                }
                else {
                    //We must use the bxxxx format
                    std::string bin = std::bitset<32>(real_val).to_string();
                    bin = bin.substr(bin.size() - sig->size);
                    this->outfile << "b" << bin << " " << sig->identifider << std::endl;
                }

                this->state[sig] = real_val;
            }
        }

        VCDSignal* create_signal(const uint8_t size, const char* wanted_name){
            // Instantiate signal
                // We copy the name bc never trust input
            char* name = (char*)malloc(sizeof(wanted_name)/sizeof(const char));
            memcpy(name, wanted_name, sizeof(wanted_name)/sizeof(const char));

            VCDSignal new_sig = {0, 0, (char)(n_signal + 64), name};

            if(size >= 32){
                new_sig.size = 32;
                new_sig.mask = 0xffffffff;
            }
            else {
                new_sig.size = size;
                new_sig.mask = (1 << size) - 1;
            }
            this->n_signal++;

            //Copy it to heap, for persistance
            VCDSignal* rep = (VCDSignal*)malloc(sizeof(VCDSignal));
            memcpy(rep, &new_sig, sizeof(VCDSignal));

            this->state[rep] = 2;

            return rep;
        }


};