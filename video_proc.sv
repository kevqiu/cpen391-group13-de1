/****************** Module for writing .bmp image *************/
/**************************************************************/ 
/*module image_write #(parameter 
WIDTH = 768, // Image width 
HEIGHT = 512, // Image height 
INFILE = "output.bmp", // Output image 
BMP_HEADER_NUM = 54 // Header for bmp image 
)
( 
input HCLK, // Clock input 
HRESETn, // Reset active low 
input hsync, // Hsync pulse 
input [7:0] DATA_WRITE_R0, // Red 8-bit data (odd) 
input [7:0] DATA_WRITE_G0, // Green 8-bit data (odd) 
input [7:0] DATA_WRITE_B0, // Blue 8-bit data (odd) 
input [7:0] DATA_WRITE_R1, // Red 8-bit data (even) 
input [7:0] DATA_WRITE_G1, // Green 8-bit data (even) 
input [7:0] DATA_WRITE_B1, // Blue 8-bit data (even) 
output reg Write_Done 
); 
//-----------------------------------// 
//-------Header data for bmp image-----// 
//-------------------------------------// 
// Windows BMP files begin with a 54-byte header: 
// Check the website to see the value of this header:
// http://www.fastgraph.com/help/bmp_header_format.html 
initial  begin 
BMP_header[ 0] = 66;BMP_header[28] =24; 
BMP_header[ 1] = 77;BMP_header[29] = 0; 
BMP_header[ 2] = 54;BMP_header[30] = 0; 
BMP_header[ 3] = 0;BMP_header[31] = 0;
BMP_header[ 4] = 18;BMP_header[32] = 0;
BMP_header[ 5] = 0;BMP_header[33] = 0; 
BMP_header[ 6] = 0;BMP_header[34] = 0; 
BMP_header[ 7] = 0;BMP_header[35] = 0; 
BMP_header[ 8] = 0;BMP_header[36] = 0; 
BMP_header[ 9] = 0;BMP_header[37] = 0; 
BMP_header[10] = 54;BMP_header[38] = 0; 
BMP_header[11] = 0;BMP_header[39] = 0; 
BMP_header[12] = 0;BMP_header[40] = 0; 
BMP_header[13] = 0;BMP_header[41] = 0; 
BMP_header[14] = 40;BMP_header[42] = 0; 
BMP_header[15] = 0;BMP_header[43] = 0; 
BMP_header[16] = 0;BMP_header[44] = 0; 
BMP_header[17] = 0;BMP_header[45] = 0; 
BMP_header[18] = 0;BMP_header[46] = 0; 
BMP_header[19] = 3;BMP_header[47] = 0;
BMP_header[20] = 0;BMP_header[48] = 0;
BMP_header[21] = 0;BMP_header[49] = 0; 
BMP_header[22] = 0;BMP_header[50] = 0; 
BMP_header[23] = 2;BMP_header[51] = 0; 
BMP_header[24] = 0;BMP_header[52] = 0; 
BMP_header[25] = 0;BMP_header[53] = 0; 
BMP_header[26] = 1; BMP_header[27] = 0; 
end
//---------------------------------------------// 
//--------------Write .bmp file ---------------// 
//---------------------------------------------// 
initial 
begin 
  fd = $fopen(INFILE, "wb+"); 
end 
 always@(Write_Done) begin 
// once the processing was done, bmp image will be created if(Write_Done == 1'b1) begin 
for(i=0; i
begin 
 $fwrite(fd, "%c", BMP_header[i][7:0]); // write the header end for(i=0; i
begin 
// write R0B0G0 and R1B1G1 (6 bytes) in a loop 
$fwrite(fd, "%c", out_BMP[i ][7:0]); 
$fwrite(fd, "%c", out_BMP[i+1][7:0]); 
$fwrite(fd, "%c", out_BMP[i+2][7:0]); 
$fwrite(fd, "%c", out_BMP[i+3][7:0]); 
$fwrite(fd, "%c", out_BMP[i+4][7:0]); 
$fwrite(fd, "%c", out_BMP[i+5][7:0]); 
end
end 
end
*/


module Color_Filter (
	input clk,
	input reset,
	input start,
	input [15:0] rgb_in,
	output reg [1:0] colour_out); //Red: 2'b01; Green: 2'b10; Blue: 2'b11
	
	reg [4:0] red;
	reg [5:0] green;
	reg [4:0] blue;
	reg [16:0] red_count, green_count, blue_count, proc_count;
	
	typedef enum {init, read, wait_next, send_colour
						} state_type;
	state_type state;
	
	always @ (posedge clk) 
	begin
		if (reset == 1'b1) 
			begin
				state <= init;
			end 
		else
			begin
				case(state)
				init:			begin
									if (start == 1'b1) 
										begin
											red_count <= 1'b0;
											green_count <= 1'b0;
											blue_count <= 1'b0;
											proc_count <= 1'b0;
											
											red <= rgb_in[15:11]; 
											green <= rgb_in[10:5];
											blue <= rgb_in[4:0];
											state <= read;
										end 
									else 
										begin
											state <= init;
										end
								end
				
				read:			begin
									if (red >= green && red >= blue) 
										begin
											red_count <= red_count + 1'b1;
										end 
									else if (green >= red && green >= blue) 
										begin
											green_count <= green_count + 1'b1;
										end 
									else 
										begin
											blue_count <= blue_count + 1'b1;
										end
									proc_count <= proc_count + 1'b1;
									state <= wait_next;
								end
								
				wait_next:	begin
									red <= rgb_in[15:11]; 
									green <= rgb_in[10:5];
									blue <= rgb_in[4:0];
									if (proc_count >= 17'b10010110000000000) 
										begin
											state <= send_colour;
										end 
									else
										begin
											state <= read;
										end
								end
								
				send_colour:begin
									if (red_count >= green_count && red_count >= blue_count) 
										begin
											colour_out <= 2'b01;
										end 
									else if (green_count >= red_count && green_count >= blue_count) 
										begin
											colour_out <= 2'b10;
										end 
									else
										begin
											colour_out <= 2'b11;
										end
									state <= init;
								end
				endcase
		end
	end
endmodule