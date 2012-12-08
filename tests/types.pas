program types(input);
var b: integer;
var a: integer;
var c: array[2..1] of real;
var d: real;
	procedure foo(b: integer; a: integer);
		procedure baz(b: integer);
		begin
			d := c[b] + b
		end;
	begin
	end;

	procedure bar(a: integer);
	begin
	end;

begin
	a := b;
	b := a;
	call bar(a, c[b])
end.
