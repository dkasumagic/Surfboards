`timescale 1ns/1ps

module surfboard #(
	parameter int W = 4,
	parameter bit SIGNED = 0
)(
	input  logic [W-1:0] A [0:3],
	input  logic [W-1:0] B [0:3],
	output logic [W-1:0] C [0:3]
);
	logic signed   [W-1:0] As [0:3];
	logic signed   [W-1:0] Bs [0:3];
	logic          [W-1:0] Au [0:3];
	logic          [W-1:0] Bu [0:3];

	assign Au = A; assign Bu = B;
	assign As = A; assign Bs = B;

	function automatic logic [W-1:0] mul(input int i, input int j);
		if (SIGNED)
			mul = As[i] * Bs[j];
		else
			mul = Au[i] * Bu[j];
	endfunction


	assign C[0] = mul(0,0) + mul(1,2);
	assign C[1] = mul(0,1) + mul(1,3);
	assign C[2] = mul(2,0) + mul(3,2);
	assign C[3] = mul(2,1) + mul(3,3);

endmodule
