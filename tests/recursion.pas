program example(input, output);
var g: integer;
procedure gcd(a: integer; b: integer);
begin
	if b = 1 then g := a
	else call gcd(b, a mod b)
end;

begin
end.
