program types(input);
var b: integer;
var a: integer;
var c: array[1..2] of real;
var d: real;
	procedure foo(b: integer);
	var a: integer;
		procedure baz(b: integer);
		begin
			d := c[b]
		end;
	begin
	end;

	procedure bar(a: integer);
	begin
	end;

begin
	a := b;
	b := a
end.
