import chisel3._

class FPAdder extends Module {
  val io = IO(new Bundle {
    val a = Input(UInt(32.W))
    val b = Input(UInt(32.W))
    val out = Output(UInt(32.W))
  })

  // Perform the IEEE 754 addition on the inputs
  val floatA = chisel3.util.fromFloat(io.a.asTypeOf(Float()))
  val floatB = chisel3.util.fromFloat(io.b.asTypeOf(Float()))
  val floatOut = floatA + floatB

  // Extracting sign, exponent, and mantissa of input1
  val s1 = io.a(31)
  val exp1 = io.a(30, 23).asSInt()
  val mantissa1 = io.a(22, 0).asUInt()

  // Extracting sign, exponent, and mantissa of input2
  val s2 = io.b(31)
  val exp2 = io.b(30, 23).asSInt()
  val mantissa2 = io.b(22, 0).asUInt()

  // Determine the larger exponent
  val largerExp = Mux(exp1 > exp2, exp1, exp2).asUInt()

  // Calculate the difference in exponents
  val expDiff = (exp1 - exp2).asUInt()

  // Shift the mantissa of the smaller number right by the difference in exponents
  val shiftedMantissa2 = Mux(exp1 > exp2, mantissa2 >> expDiff, mantissa2)

  // Add the mantissas together, being careful to handle any carry bits that may arise
  val sumMantissas = mantissa1 +& shiftedMantissa2
  val carry = sumMantissas(23)

  // If there is a carry, add the carry bit to the sum and increment the exponent
  val finalMantissa = Mux(carry, sumMantissas(22, 0) + 1.U, sumMantissas(22, 0))
  val finalExp = largerExp + Mux(carry, 1.S, 0.S)

  // Normalize the result by shifting the mantissa left or right and adjusting the exponent as needed
  val normalizedMantissa = Mux(finalMantissa(23), finalMantissa >> 1, finalMantissa << 1)
  val normalizedExp = finalExp - Mux(finalMantissa(23), 0.S, 1.S)

  // Assemble the output
  val sign = Mux(normalizedMantissa === 0.U && normalizedExp(8) === 0.U, 0.U, Mux(s1 === s2, 0.U, 1.U))
  val exponent = normalizedExp(7, 0).asUInt()
  val mantissa = normalizedMantissa(22, 0)

  // Convert the result back to a UInt
  val floatOutNorm = chisel3.util.fromFloat(Cat(sign, exponent, mantissa).asTypeOf(Float()))
  io.out := chisel3.util.toUInt(floatOutNorm)
}
