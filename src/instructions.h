#include <cstdint>

typedef uint16_t instr;

const instr instr_mask = (0b111 << 13);
const instr delay_sideset_mask = (0b11111 << 8);