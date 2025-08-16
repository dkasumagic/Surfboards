module surfboard (
	A,
	B,
	C
);
	parameter signed [31:0] W = 16;
	parameter [0:0] SIGNED = 1;
	input wire [(4 * W) - 1:0] A;
	input wire [(4 * W) - 1:0] B;
	output wire [(4 * W) - 1:0] C;
	wire signed [(4 * W) - 1:0] As;
	wire signed [(4 * W) - 1:0] Bs;
	wire [(4 * W) - 1:0] Au;
	wire [(4 * W) - 1:0] Bu;
	assign Au = A;
	assign Bu = B;
	assign As = A;
	assign Bs = B;
	function automatic [W - 1:0] mul;
		input reg signed [31:0] i;
		input reg signed [31:0] j;
		if (SIGNED)
			mul = As[(3 - i) * W+:W] * Bs[(3 - j) * W+:W];
		else
			mul = Au[(3 - i) * W+:W] * Bu[(3 - j) * W+:W];
	endfunction
	assign C[3 * W+:W] = mul(0, 0) + mul(1, 2);
	assign C[2 * W+:W] = mul(0, 1) + mul(1, 3);
	assign C[W+:W] = mul(2, 0) + mul(3, 2);
	assign C[0+:W] = mul(2, 1) + mul(3, 3);
endmodule