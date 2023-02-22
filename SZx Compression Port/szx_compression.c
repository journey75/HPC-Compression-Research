//Passing in arguments from external function----------------------------------------------------------------------------------------------------------
inline void SZ_fast_compress_args_unpredictable_one_block_float(float *oriData, size_t nbEle, float absErrBound,
                                                                unsigned char *outputBytes, int *outSize,
                                                                unsigned char *leadNumberArray_int, float medianValue,
                                                                float radius) {
    //---------------------------------------------------------------------------------------------------------
    //initialization of variables

    size_t totalSize = 0, i = 0;

    int reqLength;

    //compute median, value range, and radius

    short radExpo = getExponent_float(radius);
    computeReqLength_float(absErrBound, radExpo, &reqLength, &medianValue);

    int reqBytesLength = reqLength / 8;
    int resiBitsLength = reqLength % 8;
    int rightShiftBits = 0;
    //---------------------------------------------------------------------------------------------------------

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
    //----------------------------------------------------------------------------------------------------------
    //splitting the logic operations into multiple parts, depending of the number of bytes - can range 1-4

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
            }//---------------------------------------------------------------------------------------------------
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
            }//--------------------------------------------------------------------------------------------------
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
            }//------------------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------------------------------
//converting an array of integers into an array of bytes
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