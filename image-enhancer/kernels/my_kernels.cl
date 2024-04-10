// Local Histogram Creation Kernel
// Computes local histograms using OpenCL for parallel processing. 
// A and H are global memory buffers representing the input image and output histogram respectively.
// LH is a local memory buffer storing intermediate results for each workgroup.
// nbins specifies the number of bins for the histogram.
// is_colour is a flag indicating whether the image is colour (1) or greyscale (0).

kernel void local_histogram(global const uchar* A, global uint* H, local uint* LH, int nbins, int is_colour) {
	int id = get_global_id(0); // Global ID of the current work item
	int lid = get_local_id(0); // Local ID of the current work item
	int image_size = get_global_size(0); // Size of the image
	if (is_colour ==1){
		image_size = image_size/3; // Adjust image size for colour images
	}
	int ibins = (A[id] * nbins)/256; // Calculate the histogram bin index for the current pixel value
	int Y_Cb_Cr = id / image_size; // Determine whether the current pixel belongs to Y, Cb, or Cr component (0, 1, or 2)

	LH[lid] = 0; // Clear the local histogram bins
	
	// Process only if reading intensity value of a greyscale image or intensity component of a colour image
	if (((is_colour == 1) && (Y_Cb_Cr == 0)) || !(is_colour == 1)){
		barrier(CLK_LOCAL_MEM_FENCE); // Synchronise local memory across workgroup

		atomic_inc(&LH[ibins]); // Increment the local histogram bins corresponding to the pixel value

		barrier(CLK_LOCAL_MEM_FENCE); // Synchronise local memory across workgroups

		// Combine local histograms into the global histogram
		if (lid < nbins){ //combine all local hist into a global one
			atomic_add(&H[lid], LH[lid]);
		}
	}
}



// Global Histogram Creation Kernel
// Computes global histograms using OpenCL for parallel processing.
// A and H are global memory buffers representing the input image and output histogram respectively.
// nbins specifies the number of bins for the histogram.
// is_colour is a flag indicating whether the image is colour (1) or greyscale (0).

kernel void global_histogram(global const uchar* A, global uint* H, int nbins, int is_colour) {
	int id = get_global_id(0); // Global ID of the current work item
	int image_size = get_global_size(0); // Size of the image
	if (is_colour ==1){
		image_size = image_size / 3; // Adjust image size for colour images
	}
	int ibins = (A[id] * nbins) / 256; // Calculate the histogram bin index for the current pixel value
	int Y_Cb_Cr = id / image_size; // Determine whether the current pixel belongs to Y, Cb, or Cr component (0, 1, or 2)
	
	// Process only if reading intensity value of a greyscale image or intensity component of a colour image
	if (((is_colour == 1) && (Y_Cb_Cr == 0)) || !(is_colour == 1)){
		atomic_inc(&H[ibins]); // Increment the global histogram bin corresponding to the pixel value
	}
}



// Hillis-Steele Basic Inclusive Scan Kernel
// Performs Hillis-Steele inclusive scan algorithm on the input array A
// Requires an additional buffer B to avoid data overwrite
// A and B are global memory buffers representing the input and output arrays respectively

kernel void hillis_steele_scan(global uint* A, global uint* B) {
	int id = get_global_id(0); // Global ID of the current work item
	int N = get_global_size(0); // Size of the input array
	global int* C; // Temporary buffer pointer

	for (int stride = 1; stride <= N; stride *= 2) {
		B[id] = A[id];
		if (id >= stride)
			B[id] += A[id - stride];

		barrier(CLK_GLOBAL_MEM_FENCE); // Synchronise the step

		C = A; A = B; B = C; // Swap A & B between steps
	}
}



// Blelloch Basic Exclusive Scan Kernel
// Performs Blelloch exclusive scan algorithm on the input array A
// A is a global memory buffer representing the input array

kernel void blelloch_scan(global uint* A) {
	int id = get_global_id(0); // Global ID of the current work item
	int N = get_global_size(0); // Size of the input array
	int t; // Temporary variable for swapping

	// Up-sweep phase
	for (int stride = 1; stride < N; stride *= 2) {
		if (((id + 1) % (stride*2)) == 0)
			A[id] += A[id - stride];

		barrier(CLK_GLOBAL_MEM_FENCE); // Synchronise the step
	}

	// Down-sweep phase
	if (id == 0)
		A[N-1] = 0;// Exclusive scan

	barrier(CLK_GLOBAL_MEM_FENCE); // Synchronise the step

	for (int stride = N / 2; stride > 0; stride /= 2) {
		if (((id + 1) % (stride * 2)) == 0) {
			t = A[id];
			A[id] += A[id - stride]; // Reduce 
			A[id - stride] = t; // Move
		}

		barrier(CLK_GLOBAL_MEM_FENCE); // Synchronise the step
	}
}



// Normalize Histogram Kernel
// Normalizes the histogram using the specified scaler
// A is a global memory buffer representing the input histogram
// B is a global memory buffer representing the output histogram
// scaler is a float value used for normalization

kernel void normalise_histogram(global uint* A, global uint* B, float scaler){
	int id = get_global_id(0); // Global ID of the current work item
	B[id] = A[id] * scaler; // Normalise each bin of the histogram
}



// Backpropagation Kernel
// Performs backpropagation by equalizing the input image using the normalized histogram
// A is a global memory buffer representing the input image
// B is a global memory buffer representing the output image
// NH is a global memory buffer representing the normalized histogram
// nbins is the number of bins in the histogram
// is_colour is a flag indicating whether the image is colour (1) or greyscale (0)

kernel void equalise_image(global uchar* A, global uchar* B, global uint* NH, int nbins, int is_colour){
	int id = get_global_id(0); // Global ID of the current work item
	int image_size = get_global_size(0); // Size of the image
	if (is_colour == 1){
		image_size = image_size / 3; // Adjust image size for colour images
	}
	int ibins = (A[id] * nbins) / 256; // Calculate the histogram bin index for the current pixel value
	int Y_Cb_Cr = id / image_size; // Determine whether the current pixel belongs to Y, Cb, or Cr component (0, 1, or 2)
	
	// Adjust intensity of intensity values for colour image or intensity of greyscale image
	if (((is_colour == 1) && (Y_Cb_Cr == 0)) || !(is_colour == 1)){
		B[id] = NH[ibins]; // Equalize the pixel intensity using the normalized histogram
	}
	// Copy colour image Cb and Cr components
	else{
		B[id] = A[id];
	}
}