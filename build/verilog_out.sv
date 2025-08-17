`timescale 1ns/1ps

module surfboard #(
	parameter int W = 1,
	parameter bit SIGNED = 1
)(
	input  logic [0:3][W-1:0] A,
	input  logic [0:3][W-1:0] B,
	output logic [0:3][W-1:0] C
);
	logic signed [0:3][W-1:0] As;
	logic signed [0:3][W-1:0] Bs;
	logic        [0:3][W-1:0] Au;
	logic        [0:3][W-1:0] Bu;

	assign Au = A; assign Bu = B;
	assign As = A; assign Bs = B;

	function automatic logic [W-1:0] mul(input int i, input int j);
		if (SIGNED) mul = As[i] * Bs[j];
		else mul = Au[i] * Bu[j];
	endfunction

	assign C[0] = mul(0,0) + mul(1,2);
	assign C[1] = mul(0,1) + mul(1,3);
	assign C[2] = mul(2,0) + mul(3,2);
	assign C[3] = mul(2,1) + mul(3,3);

endmodule
