# image-processing

One of the recent innovations in computer engineering has been the development of multicore processors, which are composed of two or more independent cores in a single physical package. In this project, we have implemented parallel image processing using OpenMP.

OpenMP works as a set of pre-processor directives, runtime-library routines, and environment variables provided to the programmer, who instructs the compiler how a section of code can be multithreaded.

We have implemented three applications of image processing and computed them both sequentially and parallelly. The first application is Image Warping, which wraps any image given the coordinates. Second is Colour Inversion, which inverts the colour of an image, thus making it 'negative'. Third is Image Normalization, which changes the range of pixel intensity values.

We have done both serial and parallel computations and compared the time taken by single-threaded design and multithreaded design. This allows us to compare the performance of the system in sequential as well as in parallel. We have used OpenMP library to achieve parallelism.