import sys
import warp as wp
import numpy as np
from PIL import Image

@wp.kernel
def unsharpMaskBW(inp: wp.array(dtype=wp.float32, ndim=2), output: wp.array(dtype=wp.float32, ndim=2)): 
    i, j = wp.tid()
    output[i,j] = float(0)

    offset = kernSize // 2
    for x in range(0-offset, kernSize-offset):
        for y in range(0-offset, kernSize-offset):
            if inp[x+i,y+j]:
                output[i,j] = output[i,j] + inp[x+i,y+j]
            else:
                # get offset from this point and replace with thing in iamge
                # if past max row or col subtract how far past from max row/col
                if x+i < 0:
                    a = abs(x+i)
                elif x+i > maxRow:
                    a = x - (x-i)
                else:
                    a = x+i
                if y+j < 0:
                    b = abs(y+j)
                elif y+j > maxCol:
                    b = y - (y-j)
                else:
                    b = y+j

                output[i,j] = output[i,j] + inp[a,b]

    output[i,j] = output[i,j] / float(kernSize*kernSize)
    output[i,j] = inp[i,j] + ((inp[i,j] - output[i,j]) * scale)


@wp.kernel
def unsharpMaskColour(inp: wp.array(dtype=wp.float32, ndim=3), output: wp.array(dtype=wp.float32, ndim=3)):
    i, j, k = wp.tid()
    output[i,j,k] = float(0)

    offset = kernSize // 2
    for x in range(0-offset, kernSize-offset):
        for y in range(0-offset, kernSize-offset):
            if inp[x+i,y+j,k]:
                output[i,j,k] = output[i,j,k] + inp[x+i,y+j,k]
            else:
                if x+i < 0:
                    a = abs(x+i)
                elif x+i > maxRow:
                    a = x - (x-i)
                else:
                    a = x+i
                if y+j < 0:
                    b = abs(y+j)
                elif y+j > maxCol:
                    b = y - (y-j)
                else:
                    b = y+j

                output[i,j,k] = output[i,j,k] + inp[a,b,k]
            
    output[i,j,k] = output[i,j,k] / float(kernSize*kernSize)
    output[i,j,k] = inp[i,j,k] + ((inp[i,j,k] - output[i,j,k]) * scale)


def create_denoiseBW(N):
    @wp.kernel
    def denoiseBW(inp: wp.array(dtype=wp.float32, ndim=2), output: wp.array(dtype=wp.float32, ndim=2)):
    # sort grid, set pixel to median val
        i,j = wp.tid()
        output[i,j] = float(0)

        sortArr = wp.vector(dtype=float, length=N)
        pos = wp.int32(0)

        offset = kernSize // 2
        for x in range(0-offset, kernSize-offset):
            for y in range(0-offset, kernSize-offset):
                if inp[x+i,y+j]:
                    sortArr[pos] = inp[x+i,y+j]
                else:
                    if x+i < 0:
                        a = abs(x+i)
                    elif x+i > maxRow:
                        a = x - (x-i)
                    else:
                        a = x+i
                    if y+j < 0:
                        b = abs(y+j)
                    elif y+j > maxCol:
                        b = y - (y-j)
                    else:
                        b = y+j
                    sortArr[pos] = inp[a,b]
                
                pos = pos + 1
        
        for x in range(N):
            for y in range(0, N - x - 1):
                if sortArr[y] > sortArr[y + 1]:
                    temp = sortArr[y]
                    sortArr[y] = sortArr[y+1]
                    sortArr[y+1] = temp
                    # sortArr[y], sortArr[y + 1] = sortArr[y + 1], sortArr[y]
        
        # get median
        med = (N//2) + 1
        output[i,j] = sortArr[med]
            
    return denoiseBW

def create_denoiseCOL(N): #FINISH THIS
    @wp.kernel
    def denoiseColour(inp: wp.array(dtype=wp.float32, ndim=3), output: wp.array(dtype=wp.float32, ndim=3)):
        i,j,k = wp.tid()
        output[i,j,k] = float(0)

        sortArr = wp.vector(dtype=float, length=N)
        pos = wp.int32(0)

        offset = kernSize // 2
        for x in range(0-offset, kernSize-offset):
            for y in range(0-offset, kernSize-offset):
                if inp[x+i,y+j,k]:
                    sortArr[pos] = inp[x+i,y+j,k]
                else:
                    if x+i < 0:
                        a = abs(x+i)
                    elif x+i > maxRow:
                        a = x - (x-i)
                    else:
                        a = x+i
                    if y+j < 0:
                        b = abs(y+j)
                    elif y+j > maxCol:
                        b = y - (y-j)
                    else:
                        b = y+j
                    sortArr[pos] = inp[a,b,k]
                
                pos = pos + 1
        
        for x in range(N):
            for y in range(0, N - x - 1):
                if sortArr[y] > sortArr[y + 1]:
                    temp = sortArr[y]
                    sortArr[y] = sortArr[y+1]
                    sortArr[y+1] = temp
                    # sortArr[y], sortArr[y + 1] = sortArr[y + 1], sortArr[y]
        
        # get median
        med = (N//2) + 1
        output[i,j,k] = sortArr[med]
            
    return denoiseColour


# expected: python3 a3.py algType kernSize param inFileName outFileName
if len(sys.argv) < 6 or len(sys.argv) > 6:
    print("Bad input\n")
    exit(1)

algType = sys.argv[1]
kernSize = sys.argv[2]
param = sys.argv[3]
inFile = sys.argv[4]
outFile = sys.argv[5]

if algType != "-s" and algType != "-n":
    print("Incorrect alg type :(")
    exit(1)


image = Image.open(inFile)

# summarize some details about the image
print(image.format)
print(image.size)
print(image.mode)

numpyArr = np.asarray(image, dtype='float32')
print(numpyArr.shape)

wp.init()
wp.build.clear_kernel_cache()
device = "cpu"

inWarpImage = wp.from_numpy(numpyArr, dtype=wp.float32, device=device)
outWarpImage = wp.zeros(shape=numpyArr.shape, dtype=wp.float32, device=device)

kernSize = wp.constant(int(kernSize)) #must confirm this is positive and odd
if kernSize % 2 == 0 or kernSize < 1:
    print("Kernel size not an odd, positive integer")
    exit(1)

scale = wp.constant(float(param))

#KERNEL!!!!
# unsharp masking -sharpening
# median filter -denoise

#specify if greyscale or colour
if image.mode == "RGBA":
    image = image.convert("RGB");
    R, C, kVal = numpyArr.shape
    maxRow = wp.constant(int(R))
    maxCol = wp.constant(int(C))
    if algType == "-s":
        kernelName = unsharpMaskColour
    elif algType == "-n":
        kernelName = create_denoiseCOL(kernSize*kernSize)
        
elif image.mode == "RGB":
    R, C, kVal = numpyArr.shape
    maxRow = wp.constant(int(R))
    maxCol = wp.constant(int(C))
    if algType == "-s":
        kernelName = unsharpMaskColour
    elif algType == "-n":
        kernelName = create_denoiseCOL(kernSize*kernSize)
        
elif image.mode == "L":
    R, C = numpyArr.shape
    maxRow = wp.constant(int(R))
    maxCol = wp.constant(int(C))
    if algType == "-s":
        kernelName = unsharpMaskBW
    elif algType == "-n":
        kernelName = create_denoiseBW(kernSize*kernSize)

# launch kernel
wp.launch(kernel=kernelName, dim=numpyArr.shape, inputs=[inWarpImage, outWarpImage], device=device)
if algType == "-n":
    wp.synchronize_device()


# convert back to numpy/make image
numpyOutArr = np.asarray(wp.array.numpy(outWarpImage), dtype='float32')
numpyOutArr = np.clip(numpyOutArr, 0, 255)
imageOut = Image.fromarray(np.uint8(numpyOutArr))
imageOut.save(outFile)

image.close()
