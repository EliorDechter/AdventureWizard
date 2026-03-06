export void multiply_arrays(
    uniform float a[],
    uniform float b[],
    uniform float result[],
    uniform int count
)
{
    foreach (i = 0 ... count)
    {
        result[i] = a[i] * b[i];
    }
}