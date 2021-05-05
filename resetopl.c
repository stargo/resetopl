/* OPL2/3 reset
 *
 * Copyright (c) 2021 Michael Gernoth <michael@gernoth.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <stdio.h>
#include <io.h>

void write_opl(unsigned int base, int opl3, unsigned char reg, unsigned char v) {
	unsigned int oplbase = base;

	if (opl3) {
		oplbase += 2;
	}
	outportb(oplbase, reg);
	inportb(base);
	outportb(oplbase+1, v);
	inportb(base);
}

void set_registers(unsigned int base, unsigned char low, unsigned char high, unsigned char val, int opl3) {
	unsigned char reg;

	for (reg = low; reg <= high; reg++) {
		write_opl(base, 0, reg, val);
		if (opl3) {
			write_opl(base, 1, reg, val);
		}
	}
}

int main(int argc, char **argv) {
	unsigned int base;
	unsigned char val1, val2;
	int opl3 = 0;

	if (argc == 1) {
		base = 0x220;
		printf("No argument given, assuming base at 0x%x\n", base);
	} else if (argc == 2) {
		char *endp;
		base = strtoul(argv[1], &endp, 0);
		if (*endp != '\0') {
			fprintf(stderr, "Invalid base address %s!\n", argv[1]);
			exit(1);
		}
	}

	if (!base) {
		fprintf(stderr, "Syntax: %s base-addr\n", argv[0]);
		exit(1);
	}

	/* Detect chip */
	/* Reset timer 1 and 2 */
	write_opl(base, 0, 0x4, 0x60);

	/* Reset IRQ */
	write_opl(base, 0, 0x4, 0x80);

	/* Read status */
	val1 = inportb(base);

	/* Set timer 1 to 0xff */
	write_opl(base, 0, 0x2, 0xff);

	/* Start timer 1 */
	write_opl(base, 0, 0x4, 0x21);

	/* Delay for more than 80us, pick 10ms */
	delay(10);

	/* Read status */
	val2 = inportb(base);

	/* Reset timer 1 and 2 */
	write_opl(base, 0, 0x4, 0x60);

	/* Reset IRQ */
	write_opl(base, 0, 0x4, 0x80);

	if ((val1&0xe0) != 0x00 || (val2&0xe0) != 0xc0) {
		fprintf(stderr, "OPL not detected at 0x%x\n", base);
		exit(1);
	}
	/* Read status */
	val1 = inportb(base);

	if ((val1 & 0x06) == 0x00) {
		opl3 = 1;
	}

	printf("Resetting OPL%d at 0x%x...\n", 2+opl3, base);

	/* Tremolo/Vibrato */
	set_registers(base, 0x20, 0x35, 0, opl3);

	/* Frequency number */
	set_registers(base, 0xa0, 0xa8, 0, opl3);

	/* Key off */
	set_registers(base, 0xb0, 0xb8, 0, opl3);

	/* Drum off */
	set_registers(base, 0xbd, 0xbd, 0, opl3);

	/* Feedback */
	set_registers(base, 0xc0, 0xc8, 0, opl3);

	/* Waveform */
	set_registers(base, 0xe0, 0xf5, 0, opl3);

	/* CSW/Note SEL */
	set_registers(base, 0x08, 0x08, 0, opl3);

	/* Test/Waveform select */
	set_registers(base, 0x01, 0x01, 0, opl3);

	/* OPL2 */
	write_opl(base, 1, 4, 0);
	write_opl(base, 1, 5, 0);

	return 0;
}
