----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 07/14/2018 12:17:21 AM
-- Design Name: 
-- Module Name: TopLevelSim - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;
use ieee.std_logic_unsigned.all;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

use work.simtools.all;

entity TopLevelSim is
--  Port ( );
end TopLevelSim;

architecture Behavioral of TopLevelSim is

constant nr_outputs: natural := 18;
constant nr_inputs: natural := 24;

constant dut_nr_outputs: natural := 24;
constant dut_nr_inputs: natural := 24;

constant payload_size : natural := (3*nr_outputs)+4;
type payload is array (0 to  payload_size-1) of std_logic_vector(7 downto 0);
signal sd_out : std_logic_vector(dut_nr_outputs/2 -1  downto 0);

constant test_data: payload := 
( X"AA", X"55",
  X"55", X"AA",
   X"A0", X"B0", X"C0" --ch01 
  ,X"A1", X"B1", X"C1" --ch02
  ,X"A2", X"B2", X"C2" --ch03
  ,X"A3", X"B3", X"C3" --ch04
  ,X"A4", X"B4", X"C4" --ch05
  ,X"A5", X"B5", X"C5" --ch06
  ,X"A6", X"B6", X"C6" --ch07
  ,X"A7", X"B7", X"C7" --ch08
  ,X"A8", X"B8", X"C8" --ch09
  ,X"A9", X"B9", X"C9" --ch10
  ,X"AA", X"BA", X"CA" --ch11
  ,X"AB", X"BB", X"CB" --ch12
  ,X"AC", X"BC", X"CC" --ch13
  ,X"AD", X"BD", X"CD" --ch14
  ,X"AE", X"BE", X"CE" --ch15
  ,X"AF", X"BF", X"CF" --ch16
  ,X"D0", X"E0", X"F0" --ch17
  ,X"D1", X"E1", X"F1" --ch18
  --,X"D2", X"E2", X"F2" --ch19
  --,X"D3", X"E3", X"F3" --ch20
  --,X"D4", X"E4", X"F4" --ch21
  --,X"D5", X"E5", X"F5" --ch22
  --,X"D6", X"E6", X"F6" --ch23
  --,X"D7", X"E7", X"F7" --ch24
  --,X"D8", X"E8", X"F8" --ch25
  --,X"D9", X"E9", X"F9" --ch26
  --,X"DA", X"EA", X"FA" --ch27
  --,X"DB", X"EB", X"FB" --ch28
  --,X"DC", X"EC", X"FC" --ch29
  --,X"DD", X"ED", X"FD" --ch30
  --,X"DE", X"EE", X"FE" --ch31
  --,X"DF", X"EF", X"FF" --ch32
);

 
signal usb_clk  : std_logic;
signal reset: std_logic;


signal usb_dio : std_logic_vector(15 downto 0);
signal SLOEn  : std_logic;
signal SLRDn  : std_logic;
signal wr_counter: integer range 0 to payload_size -1;
signal out_data : std_logic_vector(15 downto 0);

signal SLWRn     : std_logic;
signal tx_full    : std_logic;

signal flaga      : std_logic;
signal flagb      : std_logic;

signal yamaha_clk: std_logic;
signal yamaha_word_clk: std_logic;

signal led : std_logic_vector(7 downto 0);
signal nr_writes : natural;

signal sof : std_logic;
signal pktend : std_logic;

begin

clk_gen(usb_clk, 48_000_000.0 );
clk_gen(yamaha_clk, 11_289_600.0 ); --44.1 khz
--clk_gen(yamaha_clk, 24_576_000.0 ); --96 khz

signal_gen(sof, usb_clk, '0', 100, '1', 8, '0', (6000-8), true);

process
begin 
	reset <= '1';
	wait for 100 ns;
	reset <= '0';
	wait;
end process;

process (usb_clk)
begin
  if rising_edge(usb_clk) then
    if reset = '1' then
      out_data <= X"EFEF";
      wr_counter <= 0;
    elsif SLRDn = '0' and flaga = '1' then
      if wr_counter = (payload_size/2-1) then
        wr_counter <= 0;
      else
        wr_counter <= wr_counter+1;
      end if;
      out_data <= test_data((wr_counter*2)+1)& test_data(wr_counter*2); -- simulate the flop clock/transfer delay
    end if;
  end if;
end process;

process (usb_clk)
  variable delay: integer := 10;
begin
  if rising_edge(usb_clk) then
    if reset = '1' then
      flaga <= '1';
    --elsif wr_counter = ((payload_size/2)-1) then
    --  flaga <= '0';
    --  delay := 10;
    --elsif delay = 0 then
    --  flaga <= '1';
    --elsif flaga = '0' then
    --  delay := delay -1;
    end if;
  end if;
end process;

process (yamaha_clk)
variable f256_count : std_logic_vector(7 downto 0);
begin
  if rising_edge(yamaha_clk) then
    if reset = '1' then
      f256_count := (others => '0');
    else
      f256_count := f256_count + X"01";
    end if;
  end if;
  yamaha_word_clk <= f256_count(7);
end process;

usb_dio <= out_data when SLOEn = '0' else (others => 'Z');

process (usb_clk, reset)
begin
  if rising_edge(usb_clk) then
    if reset = '1'then
      tx_full <= '0';
      nr_writes <= 0;
    else

      if SLWRn = '0' then
        nr_writes <= nr_writes +1;
      end if;



    end if;
  end if;
end process;

flagb <= not tx_full;

inst: entity work.CY16_to_iis
  generic map( sdi_lines => dut_nr_inputs/2, sdo_lines => dut_nr_outputs/2 )--, bit_depth => 24 )
port map(

    led(7 downto 0) => led(7 downto 0),



areset => reset,

    cy_clk  => usb_clk,
    -- DATA IO
    dio    => usb_dio,

    FLAGA => flaga, -- repeat the message
    FLAGB => flagb,
    PKTEND => pktend,
    SLRDn  => SLRDn,
    SLOEn  => SLOEn,
    SLWRn   => SLWRn,

    ain => sd_out(11 downto 0), --loopback
    aout => sd_out(11 downto 0),

    ymh_clk => yamaha_clk,
    ymh_word_clk => yamaha_word_clk,

   lsi_clk  => '0',
   lsi_mosi => '0',
   lsi_stop => '0',
   
   midi_rx => (others => '0'),

   gpio_dat => sof
    );


end Behavioral;


