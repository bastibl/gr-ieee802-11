dB2lin <- function(x) { 10^(x/10) }

gain.levels <- 0:46 * 2
gains <- dB2lin(gain.levels)

ref.level <- 1/0.03125                       # the level we want to tune to
ref.gain <- gains[which(gain.levels == 46)]  # idle gain setting

abs.level <- ref.level * ref.gain / gains

abs.level.int <- round(abs.level * 2**15)

lna.level <- c(0, 16, 30)
lna.conf <- c(0, 64, 96)

cat('if signed(power_in) > ', abs.level.int[1], ' then
     a <= std_logic_vector(to_unsigned(0, 7));\n');

last = abs.level.int[1]
for(i in 2:length(abs.level.int)) {
	if(abs.level.int[i] == last) {
		next
	}
	last = abs.level.int[i];
	gdb = gain.levels[i]

	lna.index = max(which(lna.level <= gdb))
	gdb = gdb - lna.level[lna.index]
	lna = lna.conf[lna.index]
	gdb = floor(gdb / 2)

cat('elsif signed(power_in) > ', last, ' then
     a <= std_logic_vector(to_unsigned(', lna + gdb, ', 7));\n');
}


#    --if signed(power_in) > 2**24 then
#    --    a <= std_logic_vector(to_unsigned(0, 7));
#    --elsif signed(power_in) > 2**16 then
#    --    a <= std_logic_vector(to_unsigned(32, 7));
#    --else
#    --    a <= std_logic_vector(to_unsigned(127, 7));
#    --end if;
