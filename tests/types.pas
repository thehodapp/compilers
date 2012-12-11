program types(input);
var b: integer;
var a: integer;
var c: array[1..2] of real;
var d: real;
	procedure foo(b: integer; a: integer);
		procedure baz(b: integer);
		begin
			d := c[b] + b
		end;
	begin
	end;

	procedure bar(a: array[1..2] of real);
	begin
	end;

begin
	a := b;
	b := a;
	call bar(c)
end.
