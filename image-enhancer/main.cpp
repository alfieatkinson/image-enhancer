#include <iostream>
#include <vector>

#include "Utils.h"
#include "CImg.h"

using namespace cimg_library;


void print_help() {
	std::cerr << "Application usage:" << std::endl;

	std::cerr << "  -p : select platform " << std::endl;
	std::cerr << "  -d : select device" << std::endl;
	std::cerr << "  -l : list all platforms and devices" << std::endl;
	//std::cerr << "  -f : input image file (default: test.pgm)" << std::endl;
	std::cerr << "  -h : print this message" << std::endl;
}



// The following function allows the user to select which of the test images to use the program on.
// It also allows them to input the path to their own image to use it on, with a try/catch block for error handling.
// Returns a pair containing a string (filename) and a bool (whether it is a colour image).

std::pair<string, bool> choose_image() {
	string choice; // String user input
	int nchoice; // Integer user input

	std::pair<string, bool> img; // Filename and is colour pair to be returned

	size_t dot_position; // Position of the last dot in filename

	// Enter a while loop to allow the user to retry after erroneous input
	bool loop = true; // Should keep looping?
	while (loop) {
		std::vector<std::string> options = { // Options to display to the user
			"Colour",
			"Greyscale",
			"Large Colour",
			"Large Greyscale",
			"Custom Image"
		};

		std::cout << "Which image would you like to use?" << std::endl;

		// Display the options with numbered list
		for (int i = 0; i < options.size(); ++i) {
			std::cout << i + 1 << ". " << options[i] << std::endl;
		}

		std::cin >> choice;

		// Check that the user input is an integer within the accepted bounds
		try {
			nchoice = stoi(choice);
			switch (nchoice) {
			case 1:
				img.first = "test.ppm";
				img.second = true;

				loop = false;
				break;
			case 2:
				img.first = "test.pgm";
				img.second = false;

				loop = false;
				break;
			case 3:
				img.first = "test_large.ppm";
				img.second = true;

				loop = false;
				break;
			case 4:
				img.first = "test_large.pgm";
				img.second = false;

				loop = false;
				break;
			case 5:
				std::cout << "Input file location of image" << std::endl;
				std::cin >> choice;
				img.first = choice;

				// Check file extension to determine if image is compatible file extension and greyscale or colour
				dot_position = img.first.find_last_of(".");
				if (dot_position != string::npos) {
					string extension = img.first.substr(dot_position + 1);

					if (extension == "ppm") {
						img.second = true;
					}
					else if (extension == "pgm") {
						img.second = false;
					}
					else {
						std::cout << "Unsupported file format. Please provide a PPM or PGM file" << std::endl;
						break;
					}
				}
				else {
					std::cout << "Could not determine file format from the extension." << std::endl;
					break;
				}

				loop = false;
				break;
			default:
				std::cout << "Invalid Integer" << std::endl;
			}
		}
		catch (...) {
			std::cout << "Please input a valid integer" << std::endl;
		}
	}
	return img;
}



// The following function allows the user to select a number of bins from 2 to 256
// There is a try/catch loop for error handling
// Returns an integer (number of bins)

int choose_bins() {
	string choice; // String user input
	int nchoice; // Integer user input

	int nbins; // Number of bins to be returned

	// Enter a while loop to allow the user to retry after erroneous input
	bool loop = true; // Should keep looping?
	while (loop) {
		std::cout << "How many bins?" << std::endl;

		// Print powers of 2 up to 256 as a list
		for (int i = 1; i <= 256; i *= 2) {
			int index = std::log2(i);
			std::cout << index + 1 << ". " << i << std::endl;
		}

		std::cin >> choice;

		// Check that the user input is an integer within the accepted bounds
		try {
			nchoice = stoi(choice);
			if ((nchoice >= 1) && (nchoice <= 9)) {
				nbins = pow(2, nchoice);
				loop = false;
			}
			else {
				std::cout << "Invalid Integer" << std::endl;
			}
		}
		catch (...) {
			std::cout << "Please input a valid integer" << std::endl;
		}
	}
	return nbins;
}



// The following function asks the user whether they would like to use global or local memory
// It also uses a try/catch block for error handling
// Returns bool true (local) or false (global)

bool choose_memory() {
	string choice; // String user input
	int nchoice; // Integer user input
	
	bool local; // Local or global memory? To be returned

	// Enter a while loop to allow the user to retry after erroneous input
	bool loop = true; // Should keep looping?
	while (loop) {
		std::cout << "Global or local memory?" << std::endl << "1. Global" << std::endl << "2. Local" << std::endl;
		std::cin >> choice;

		// Check that the user input is an integer within the accepted bounds
		try {
			nchoice = stoi(choice);
			switch (nchoice) {
			case 1:
				local = false;
				loop = false;
				break;
			case 2:
				local = true;
				loop = false;
				break;
			default:
				std::cout << "Invalid Integer" << std::endl;
			}
		}
		catch (...) {
			std::cout << "Please input a valid integer" << std::endl;
		}
	}
	return local;
}



// The following function asks the user whether they would like to use Blelloch or Hillis-Steele cumulation
// It also uses a try/catch block for error handling
// Returns a string "bl" for Blelloch or "hs" for Hillis-Steele

string choose_cumulation() {
	string choice; // String user input
	int nchoice; // Integer user input
	
	string cumulation; // Type of cumulation to be returned

	// Enter a while loop to allow the user to retry after erroneous input
	bool loop = true; // Should keep looping?
	while (loop) {
		std::cout << "Would you like to use Blelloch or Hillis-Steele cumulation?" << std::endl << "1. Blelloch" << std::endl << "2. Hillis-Steele" << std::endl;
		std::cin >> choice;
		try {
			nchoice = stoi(choice);
			switch (nchoice) {
			case 1:
				cumulation = "bl";
				loop = false;
				break;
			case 2:
				cumulation = "hs";
				loop = false;
				break;
			default:
				std::cout << "Invalid Integer" << std::endl;
			}
		}
		catch (...) {
			std::cout << "Please input a valid integer" << std::endl;
		}
	}
	return cumulation;
}



// Main function which handles kernel setup and execution

int main(int argc, char **argv) {
	// Handle command line options such as device selection, verbosity, etc.
	int platform_id = 0;
	int device_id = 0;

	for (int i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "-p") == 0) && (i < (argc - 1))) { platform_id = atoi(argv[++i]); }
		else if ((strcmp(argv[i], "-d") == 0) && (i < (argc - 1))) { device_id = atoi(argv[++i]); }
		else if (strcmp(argv[i], "-l") == 0) { std::cout << ListPlatformsDevices() << std::endl; }
		//else if ((strcmp(argv[i], "-f") == 0) && (i < (argc - 1))) { img_path = argv[++i]; }
		else if (strcmp(argv[i], "-h") == 0) { print_help(); return 0; }
	}

	// Run functions for user selections
	std::pair<string, bool> img_info = choose_image();
	string img_path = img_info.first;
	bool is_colour = img_info.second;
	int nbins = choose_bins();
	bool local = choose_memory();
	string cumulation = choose_cumulation();

	// Kernels cannot accept bool values, so an int is created with same value
	int ncolour = (is_colour) ? 1 : 0;

	vector<unsigned int> histogram(nbins, 0);
	size_t total_memory = nbins * sizeof(unsigned int);

	cimg::exception_mode(0);



	// Try/catch for error handling
	try {
		// Setup image to be displayed and display input image
		CImg<unsigned char> img_in(img_path.c_str());
		CImgDisplay disp_input(img_in,"input");

		float image_size = img_in.size(); // Get image size
		if (is_colour) img_in.RGBtoYCbCr(); // If it's a colour image, change to YCbCr
		




		// Host operations

		cl::Context context = GetContext(platform_id, device_id); // Select computing devices
		std::cout << "Running on " << GetPlatformName(platform_id) << ", " << GetDeviceName(platform_id, device_id) << std::endl; // Display the selected device

		// Create a queue to push commands to for the device
		cl::CommandQueue queue(context, CL_QUEUE_PROFILING_ENABLE); // Enable queue profiling to get kernel execution times
		
		// Load & build the device code
		cl::Program::Sources sources; 
		AddSources(sources, "kernels/my_kernels.cl");
		cl::Program program(context, sources);

		// Build and debug the kernel code
		try { 
			program.build();
		}
		catch (const cl::Error& err) {
			std::cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(context.getInfo<CL_CONTEXT_DEVICES>()[0]) << std::endl;
			std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(context.getInfo<CL_CONTEXT_DEVICES>()[0]) << std::endl;
			std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(context.getInfo<CL_CONTEXT_DEVICES>()[0]) << std::endl;
			throw err;
		}





		// Device operations

		cl::Buffer dev_img_in(context, CL_MEM_READ_ONLY, img_in.size()); // Create image input buffer
		cl::Buffer dev_img_out(context, CL_MEM_READ_WRITE, img_in.size()); // Create image output buffer

		cl::Buffer dev_histogram(context, CL_MEM_READ_WRITE, total_memory); // Create histogram buffer
		cl::Buffer dev_cumulative(context, CL_MEM_READ_WRITE, total_memory); // Create cumulative histogram buffer
		cl::Buffer dev_normalise(context, CL_MEM_READ_WRITE, total_memory); // Create normalised histogram buffer
		
		queue.enqueueWriteBuffer(dev_img_in, CL_TRUE, 0, img_in.size(), &img_in.data()[0]); // Copy image to dev_img_in





		// Histogram 

		cl::Kernel histogram_kernel; // Setup and execute histogram kernel 
		cl::Event histogram_timer; // Create event to get execution time

		// If the histogram is local, use local kernel and set args for it
		if (local) {

			histogram_kernel = cl::Kernel(program, "local_histogram");
			histogram_kernel.setArg(0, dev_img_in); // Input image
			histogram_kernel.setArg(1, dev_histogram); // Histogram
			histogram_kernel.setArg(2, cl::Local(total_memory)); // Memory for local histogram
			histogram_kernel.setArg(3, nbins); // Number of bins
			histogram_kernel.setArg(4, ncolour); // If image is colour or not
		}
		// If the histogram is global, use global kernel and set args for it
		else {
			histogram_kernel = cl::Kernel(program, "global_histogram");
			histogram_kernel.setArg(0, dev_img_in); // Input image
			histogram_kernel.setArg(1, dev_histogram); // Histogram
			histogram_kernel.setArg(2, nbins); // Number of bins
			histogram_kernel.setArg(3, ncolour); // If image is colour or not
		}

		// Execute the chosen histogram kernel
		queue.enqueueNDRangeKernel(histogram_kernel, cl::NullRange, cl::NDRange(img_in.size()), cl::NDRange(nbins), NULL, &histogram_timer);
		queue.enqueueReadBuffer(dev_histogram, CL_TRUE, 0, total_memory, &histogram[0], NULL);

		// Output kernel execution time and add to total
		int histogram_time_ns = histogram_timer.getProfilingInfo<CL_PROFILING_COMMAND_END>() - histogram_timer.getProfilingInfo<CL_PROFILING_COMMAND_START>();
		int total_timer = histogram_time_ns;
		std::cout << "Histogram kernel execution time [ns]:" << histogram_time_ns << std::endl;

		// Output simple histogram
		std::cout << "Simple Histogram:" << endl;
		std::cout << histogram << endl;





		// Cumulative histogram

		cl::Kernel cumulative_kernel; // Setup and execute cumulative histogram kernel 
		cl::Event cumulative_timer; // Create event to get execution time

		// Set args for Hillis-Steele kernel
		if (cumulation == "hs") {
			cumulative_kernel = cl::Kernel(program, "hillis_steele_scan");
			cumulative_kernel.setArg(0, dev_histogram); // Histogram
			cumulative_kernel.setArg(1, dev_cumulative); // Cumulative Histogram
		}
		// Set args for Blelloch kernel
		else if (cumulation == "bl") {
			cumulative_kernel = cl::Kernel(program, "blelloch_scan");
			cumulative_kernel.setArg(0, dev_histogram); // Histogram
			dev_cumulative = dev_histogram; // Set cumulative histogram to histogram as Blelloch only needs one arg
		}

		// Execute the chosen histogram scanning kernel
		queue.enqueueNDRangeKernel(cumulative_kernel, cl::NullRange, cl::NDRange(nbins), cl::NDRange(nbins), NULL, &cumulative_timer);
		queue.enqueueReadBuffer(dev_cumulative, CL_TRUE, 0, total_memory, &histogram[0], NULL);

		// Output kernel execution time and add to total
		int c_histogram_time_ns = cumulative_timer.getProfilingInfo<CL_PROFILING_COMMAND_END>() - cumulative_timer.getProfilingInfo<CL_PROFILING_COMMAND_START>();
		total_timer += c_histogram_time_ns;
		std::cout << "Cummulative histogram kernel execution time [ns]:" << c_histogram_time_ns << std::endl;

		// Output cumulative histogram
		std::cout << "Cumulative Histogram:" << std::endl;
		std::cout << histogram << std::endl;
		

		


		// Normalisation kernel

		float scale = 255.0f / image_size; // Get scaler to normalise cumulative histogram with

		// Scaler is three times bigger for colour
		if (is_colour) {
			scale = scale * 3;
		}

		// Create event to get execution time
		cl::Event normalise_timer;

		// Get kernel and set args
		cl::Kernel normalise_kernel = cl::Kernel(program, "normalise_histogram");
		normalise_kernel.setArg(0, dev_cumulative); // Cumulative Histogram
		normalise_kernel.setArg(1, dev_normalise); // Normalised Histogram 
		normalise_kernel.setArg(2, scale); // What to scale

		// Execute the histogram normalisation kernel
		queue.enqueueNDRangeKernel(normalise_kernel, cl::NullRange, cl::NDRange(nbins), cl::NDRange(nbins), NULL, &normalise_timer);
		queue.enqueueReadBuffer(dev_normalise, CL_TRUE, 0, total_memory, &histogram[0], NULL);

		// Output kernel execution time and add to total
		int n_histogram_time_ns = normalise_timer.getProfilingInfo<CL_PROFILING_COMMAND_END>() - normalise_timer.getProfilingInfo<CL_PROFILING_COMMAND_START>();
		total_timer += n_histogram_time_ns;
		std::cout << "Normalised histogram kernel execution time [ns]:" << n_histogram_time_ns << std::endl;

		// Output normalised histogram
		std::cout << "Normalised Histogram:" << std::endl;
		std::cout << histogram << std::endl;





		// Image equalisation

		cl::Event equalisation_timer; // Create event to get execution time

		// Get kernel and set args
		cl::Kernel equalise_kernel = cl::Kernel(program, "equalise_image");
		equalise_kernel.setArg(0, dev_img_in); // Input image 
		equalise_kernel.setArg(1, dev_img_out); // Output image 
		equalise_kernel.setArg(2, dev_normalise); // Normalised histogram
		equalise_kernel.setArg(3, nbins); // Number of bins
		equalise_kernel.setArg(4, ncolour); // If image is colour or not

		// Execute the image equalisation kernel
		queue.enqueueNDRangeKernel(equalise_kernel, cl::NullRange, cl::NDRange(img_in.size()), cl::NDRange(nbins), NULL, &equalisation_timer);
		vector<unsigned char> out_buffer(img_in.size());
		queue.enqueueReadBuffer(dev_img_out, CL_TRUE, 0, out_buffer.size(), &out_buffer.data()[0], NULL);

		// Output kernel execution time and add to total
		int equalisation_time_ns = equalisation_timer.getProfilingInfo<CL_PROFILING_COMMAND_END>() - equalisation_timer.getProfilingInfo<CL_PROFILING_COMMAND_START>();
		total_timer += equalisation_time_ns;
		std::cout << "Equalisation kernel execution time [ns]:" << equalisation_time_ns << std::endl;

		// Output total run time
		std::cout << "Total execution time [ns]:" << total_timer << std::endl;

		// Get data for output image
		CImg<unsigned char> img_out(out_buffer.data(), img_in.width(), img_in.height(), img_in.depth(), img_in.spectrum());

		// If colour, convert back from YCbCr to RGB
		if (is_colour == 1) img_out.YCbCrtoRGB();





		// Output image
		CImgDisplay disp_output(img_out, "output"); 

		// Keep images displayed
		while (!disp_input.is_closed() && !disp_output.is_closed()
			&& !disp_input.is_keyESC() && !disp_output.is_keyESC()) {
			disp_input.wait(1);
			disp_output.wait(1);
		}
	}
	catch (const cl::Error& err) {
		std::cerr << "ERROR: " << err.what() << ", " << getErrorString(err.err()) << std::endl;
	}
	catch (CImgException& err) {
		std::cerr << "ERROR: " << err.what() << std::endl;
	}

	return 0;
}
