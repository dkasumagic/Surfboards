`timescale 1ns/1ps

module surfboard #(
	parameter int W = 4,
	parameter bit SIGNED = 1
)(
	input  logic [W-1:0] A [0:8],
	input  logic [W-1:0] B [0:8],
	output logic [W-1:0] C [0:8]
);
	logic signed   [W-1:0] As [0:8];
	logic signed   [W-1:0] Bs [0:8];
	logic          [W-1:0] Au [0:8];
	logic          [W-1:0] Bu [0:8];

	assign Au = A; assign Bu = B;
	assign As = A; assign Bs = B;

	function automatic logic [W-1:0] mul(input int i, input int j);
		if (SIGNED)
			mul = As[i] * Bs[j];
		else
			mul = Au[i] * Bu[j];
	endfunction
	assign C[0] = mul(0,0) + mul(1,3) + mul(2,6);
	assign C[1] = mul(0,1) + mul(1,4) + mul(2,7);
	assign C[2] = mul(0,2) + mul(1,5) + mul(2,8);
	assign C[3] = mul(3,0) + mul(4,3) + mul(5,6);
	assign C[4] = mul(3,1) + mul(4,4) + mul(5,7);
	assign C[5] = mul(3,2) + mul(4,5) + mul(5,8);
	assign C[6] = mul(6,0) + mul(7,3) + mul(8,6);
	assign C[7] = mul(6,1) + mul(7,4) + mul(8,7);
	assign C[8] = mul(6,2) + mul(7,5) + mul(8,8);

endmodule
