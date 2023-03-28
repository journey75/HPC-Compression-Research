inline short getPrecisionReqLength_float(float precision)
{
	lfloat lbuf;
	lbuf.value = precision;
	int ivalue = lbuf.ivalue;
	
	int expValue = (ivalue & 0x7F800000) >> 23;
	expValue -= 127;
//	unsigned char the1stManBit = (unsigned char)((ivalue & 0x00400000) >> 22);
//	if(the1stManBit==1)
//		expValue--;	
	return (short)expValue;
}

inline void computeReqLength_float(double realPrecision, short radExpo, int *reqLength, float *medianValue) {
    short reqExpo = getPrecisionReqLength_double(realPrecision);
    *reqLength = 9 + radExpo - reqExpo + 1; //radExpo-reqExpo == reqMantiLength
    if (*reqLength < 9)
        *reqLength = 9;
    if (*reqLength > 32) {
        *reqLength = 32;
        *medianValue = 0;
    }
}

inline void SZ_fast_compress_args_unpredictable_one_block_float(float *oriData, size_t nbEle, float absErrBound,
                                                                unsigned char *outputBytes, int *outSize,
                                                                unsigned char *leadNumberArray_int, float medianValue,
                                                                float radius) {
    size_t totalSize = 0, i = 0;

    int reqLength;

    //compute median, value range, and radius

    short radExpo = getExponent_float(radius);
    computeReqLength_float(absErrBound, radExpo, &reqLength, &medianValue);

    int reqBytesLength = reqLength / 8;
    int resiBitsLength = reqLength % 8;
    int rightShiftBits = 0;

    size_t leadNumberArray_size = nbEle % 4 == 0 ? nbEle / 4 : nbEle / 4 + 1;

    register lfloat lfBuf_pre;
    register lfloat lfBuf_cur;
    lfBuf_pre.ivalue = 0;

    unsigned char *leadNumberArray = outputBytes + 1 + sizeof(float);

    unsigned char *exactMidbyteArray = leadNumberArray + leadNumberArray_size;

    if (resiBitsLength != 0) {
        rightShiftBits = 8 - resiBitsLength;
        reqBytesLength++;
    }

    register unsigned char leadingNum = 0;
    size_t residualMidBytes_size = 0;
    if (sysEndianType == LITTLE_ENDIAN_SYSTEM) {
        if (reqBytesLength == 2) {
            for (i = 0; i < nbEle; i++) {
                leadingNum = 0;
                lfBuf_cur.value = oriData[i] - medianValue;

                lfBuf_cur.ivalue = lfBuf_cur.ivalue >> rightShiftBits;

                lfBuf_pre.ivalue = lfBuf_cur.ivalue ^ lfBuf_pre.ivalue;

                if (lfBuf_pre.ivalue >> 8 == 0)
                    leadingNum = 3;
                else if (lfBuf_pre.ivalue >> 16 == 0)
                    leadingNum = 2;
                else if (lfBuf_pre.ivalue >> 24 == 0)
                    leadingNum = 1;

                leadNumberArray_int[i] = leadingNum;

                if (leadingNum == 0) {
                    exactMidbyteArray[residualMidBytes_size] = lfBuf_cur.byte[2];
                    exactMidbyteArray[residualMidBytes_size + 1] = lfBuf_cur.byte[3];
                    residualMidBytes_size += 2;
                } else if (leadingNum == 1) {
                    exactMidbyteArray[residualMidBytes_size] = lfBuf_cur.byte[2];
                    residualMidBytes_size++;
                }

                lfBuf_pre = lfBuf_cur;
            }
        } else if (reqBytesLength == 3) {
            for (i = 0; i < nbEle; i++) {
                leadingNum = 0;
                lfBuf_cur.value = oriData[i] - medianValue;

                lfBuf_cur.ivalue = lfBuf_cur.ivalue >> rightShiftBits;

                lfBuf_pre.ivalue = lfBuf_cur.ivalue ^ lfBuf_pre.ivalue;

                if (lfBuf_pre.ivalue >> 8 == 0)
                    leadingNum = 3;
                else if (lfBuf_pre.ivalue >> 16 == 0)
                    leadingNum = 2;
                else if (lfBuf_pre.ivalue >> 24 == 0)
                    leadingNum = 1;

                leadNumberArray_int[i] = leadingNum;

                if (leadingNum == 0) {
                    exactMidbyteArray[residualMidBytes_size] = lfBuf_cur.byte[1];
                    exactMidbyteArray[residualMidBytes_size + 1] = lfBuf_cur.byte[2];
                    exactMidbyteArray[residualMidBytes_size + 2] = lfBuf_cur.byte[3];
                    residualMidBytes_size += 3;
                } else if (leadingNum == 1) {
                    exactMidbyteArray[residualMidBytes_size] = lfBuf_cur.byte[1];
                    exactMidbyteArray[residualMidBytes_size + 1] = lfBuf_cur.byte[2];
                    residualMidBytes_size += 2;
                } else if (leadingNum == 2) {
                    exactMidbyteArray[residualMidBytes_size] = lfBuf_cur.byte[1];
                    residualMidBytes_size++;
                }

                lfBuf_pre = lfBuf_cur;
            }
        } else if (reqBytesLength == 1) {
            for (i = 0; i < nbEle; i++) {
                leadingNum = 0;
                lfBuf_cur.value = oriData[i] - medianValue;

                lfBuf_cur.ivalue = lfBuf_cur.ivalue >> rightShiftBits;

                lfBuf_pre.ivalue = lfBuf_cur.ivalue ^ lfBuf_pre.ivalue;

                if (lfBuf_pre.ivalue >> 8 == 0)
                    leadingNum = 3;
                else if (lfBuf_pre.ivalue >> 16 == 0)
                    leadingNum = 2;
                else if (lfBuf_pre.ivalue >> 24 == 0)
                    leadingNum = 1;

                leadNumberArray_int[i] = leadingNum;

                if (leadingNum == 0) {
                    exactMidbyteArray[residualMidBytes_size] = lfBuf_cur.byte[3];
                    residualMidBytes_size++;
                }

                lfBuf_pre = lfBuf_cur;
            }
        } else //reqBytesLength == 4
        {
            for (i = 0; i < nbEle; i++) {
                leadingNum = 0;
                lfBuf_cur.value = oriData[i] - medianValue;

                lfBuf_cur.ivalue = lfBuf_cur.ivalue >> rightShiftBits;

                lfBuf_pre.ivalue = lfBuf_cur.ivalue ^ lfBuf_pre.ivalue;

                if (lfBuf_pre.ivalue >> 8 == 0)
                    leadingNum = 3;
                else if (lfBuf_pre.ivalue >> 16 == 0)
                    leadingNum = 2;
                else if (lfBuf_pre.ivalue >> 24 == 0)
                    leadingNum = 1;

                leadNumberArray_int[i] = leadingNum;

                if (leadingNum == 0) {
                    exactMidbyteArray[residualMidBytes_size] = lfBuf_cur.byte[0];
                    exactMidbyteArray[residualMidBytes_size + 1] = lfBuf_cur.byte[1];
                    exactMidbyteArray[residualMidBytes_size + 2] = lfBuf_cur.byte[2];
                    exactMidbyteArray[residualMidBytes_size + 3] = lfBuf_cur.byte[3];
                    residualMidBytes_size += 4;
                } else if (leadingNum == 1) {
                    exactMidbyteArray[residualMidBytes_size] = lfBuf_cur.byte[0];
                    exactMidbyteArray[residualMidBytes_size + 1] = lfBuf_cur.byte[1];
                    exactMidbyteArray[residualMidBytes_size + 2] = lfBuf_cur.byte[2];
                    residualMidBytes_size += 3;
                } else if (leadingNum == 2) {
                    exactMidbyteArray[residualMidBytes_size] = lfBuf_cur.byte[0];
                    exactMidbyteArray[residualMidBytes_size + 1] = lfBuf_cur.byte[1];
                    residualMidBytes_size += 2;
                } else //leadingNum == 3
                {
                    exactMidbyteArray[residualMidBytes_size] = lfBuf_cur.byte[0];
                    residualMidBytes_size++;
                }

                lfBuf_pre = lfBuf_cur;
            }
        }

        convertIntArray2ByteArray_fast_2b_args(leadNumberArray_int, nbEle, leadNumberArray);
        int k = 0;

        unsigned char reqLengthB = (unsigned char) reqLength;
        outputBytes[k] = reqLengthB;
        k++;
        floatToBytes(&(outputBytes[k]), medianValue);
        k += sizeof(float);
        //sizeToBytes(&(outputBytes[k]), leadNumberArray_size);
        //outputBytes[k] = leadNumberArray_size;  //leadNumberArray_size can be calculated based on block size (=blockSize/4)

        totalSize = 1 + sizeof(float) + leadNumberArray_size + residualMidBytes_size;
    } else {

    }

    *outSize = totalSize;

}


//1. Define a module for our floating-point adder that takes two 32-bit IEEE 754 floating-point (in our case Uint) numbers as input and produces a 32-bit IEEE 754 floating-point (in our case Uint) number as output.

//2. Create input and output ports for our module that match the input and output types described in step 1.

//3. Split the input numbers into their component parts (sign, exponent, and mantissa).

//4. Determine the larger of the two exponents, and shift the mantissa of the smaller number right by the difference in exponents to align the decimal points.

//5. Add the mantissas together, being careful to handle any carry bits that may arise.

//6. Normalize the result by shifting the mantissa left or right and adjusting the exponent as needed.

//7. Combine the sign, exponent, and mantissa into a single 32-bit IEEE 754 floating-point number.

//8. Output the result through the output port.
