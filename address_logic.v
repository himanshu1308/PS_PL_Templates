`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 27.11.2023 01:38:30
// Design Name: 
// Module Name: address_logic
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module address_logic(
    input clk, 
    input done,
    output reg [15:0]pl_addr_logic,
    output reg pl_en,
    output reg pl_wr_en
    
    );

    
    
    initial begin
        pl_addr_logic <= 'd0;
        pl_en <= 'd0;
        pl_wr_en <= 'd0;
    end
    
    always @(posedge clk) begin
        if(done) begin
            pl_en <= 1'b1;       
            if(pl_addr_logic == 'd65535)
                pl_addr_logic <= 0;
            else
                pl_addr_logic <= pl_addr_logic+1;           
        end
        else
            pl_en<=1'b0;

    end
    
    
    
    
endmodule
