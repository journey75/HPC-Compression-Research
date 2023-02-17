import chisel3._

class MyModule extends Module 
{
  val io = IO(new Bundle 
  {

  })

  val myArray = RegInit(VecInit(Seq.fill(128)(0.U(32.W))))
  // Initialize a 128-entry array of 32-bit numbers with all zeroes

  // Set the first entry in the array to some non-zero value - this is a hardcode, but will be set by the data read in
  myArray(0) := 123.U(32.W)

  val baselineBits = RegInit(VecInit(Seq.fill(32)(false.B)))
  // Initialize a 32-entry array of Booleans with all false values to ensure there wont be redundant bits

  // Extract the bits of the first entry and set the corresponding bits in the baselineBits array to true
  for (i <- 0 until 32) 
  {
    baselineBits(i) := myArray(0)(i)
  }

  val hasRepeatingBits = RegInit(VecInit(Seq.fill(127)(false.B)))
  // Initialize a 127-entry array of Booleans with all false values

  // Loop through the rest of the array (starting from index 1)
  for (i <- 1 until 128) 
  {
    val currBits = Wire(Vec(32, Bool()))
    // Create a temporary wire to hold the bits of the current entry

    for (j <- 0 until 32) 
    {
      currBits(j) := myArray(i)(j)
    }
    // Extract the bits of the current entry and assign them to currBits

    hasRepeatingBits(i-1) := currBits === baselineBits
    // Compare currBits to baselineBits and set hasRepeatingBits(i-1) to true if they are equal
  }

  // Other code can manipulate myArray, baselineBits, and hasRepeatingBits as needed

}
