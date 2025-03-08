#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RAND_DOUBLE() (double)rand()/(double)(RAND_MAX)

void printUsage(const char* programName) {
	printf("usage: %s <width> <height> <density> <steps>\n", programName);
}

int main(int argc, char* argv[]) {
	if(argc != 5) {
		printUsage(argv[0]);
		return EXIT_FAILURE;
	}

	const int width = atoi(argv[1]);
	const int height = atoi(argv[2]);
	const double density = atof(argv[3]);
	const int steps = atoi(argv[4]);

	printf("width:   %4d\n", width);
	printf("height:  %4d\n", height);
	printf("density: %4.0f%%\n", density * 100);
	printf("steps:   %4d\n", steps);

	// Seeding the random number generator so we get a different starting field
	// every time.
	srand(time(NULL));

	// TODO
	// Initalize 2 Fields
	_Bool (*pictures[2])[height] = {malloc(width * sizeof(*pictures[0])), malloc(width * sizeof(*pictures[1]))};

    if (!pictures[0] || !pictures[1]) {
        printf("Memory Error\n");
        free(pictures[0]);
        free(pictures[1]);
        return EXIT_FAILURE;
    }

	char filename[14]; // gol_XXXXX.pbm has 14 char inc. \0

	//Init
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			pictures[0][x][y] = (RAND_DOUBLE() <= density) ? 1 : 0;
		}
	}

	//Steps
	int next_live_cells;
	int current_cell_lives;

	for (int step = 0; step <= steps; step++) {
		sprintf(filename, "gol_%05d.pbm", step);
		FILE *pbm = fopen(filename, "w");
		if (!pbm) {
			perror("File Error\n");
			return EXIT_FAILURE;
		}
		fprintf(pbm, "P1\n%d %d\n", width, height);

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				fprintf(pbm, "%d ", pictures[step % 2][x][y]);
				next_live_cells = 0;
				current_cell_lives = pictures[step % 2][x][y];

				//Calc neighbour cells
				for (int ny = ((y-1) < 0) ? 0 : (y-1); ny <= (((y+1) > (height-1)) ? (height-1) : (y+1)); ny++) {
					for (int nx = ((x-1) < 0) ? 0 : (x-1); nx <= (((x+1) > (width-1)) ? (width-1) : (x+1)); nx++) {
						if (ny == y && nx == x) {
							continue;
						}
						next_live_cells += pictures[step % 2][nx][ny];
					}
				}
				
				//if (next_live_cells) printf("Cell %d, %d has %d neighbours.\n", x, y, next_live_cells);
				if (current_cell_lives && ((next_live_cells < 2) || (next_live_cells > 3))) {
					pictures[(step + 1) % 2][x][y] = 0;
				}
				else if (current_cell_lives && ((next_live_cells == 2) || (next_live_cells == 3))) {
					pictures[(step + 1) % 2][x][y] = 1;
				}
				else if (!current_cell_lives && (next_live_cells == 3)) {
					pictures[(step + 1) % 2][x][y] = 1;
				}
				else {
					pictures[(step + 1) % 2][x][y] = 0;
				}
			}
			fprintf(pbm, "\n");
		}
		fclose(pbm);
	}
	free(pictures[0]);
	free(pictures[1]);
	return EXIT_SUCCESS;
}
