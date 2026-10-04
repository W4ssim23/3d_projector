#include <stdlib.h>
#include <stdio.h>

#include <math.h>

#include "Utils/imageFormationUtils.h"
#include "Utils/Util.h"

int main(int argc, char *argv[])
{
    if (argc != 17 && argc != 16)
    {
        printf("\nUsage: %s off_file_name.off "
               "f width height u_0 v_0 alpha_u alpha_v "
               "gama beta alpha T_x T_y T_z "
               "use_depth "
               "[out_pam]\n",
               argv[0]);
        exit(0);
    }

    // Reading the input parameters
    char *off_filename = argv[1];
    // Intrinsics
    float f = atof(argv[2]);

    int width = atoi(argv[3]);
    int height = atoi(argv[4]);

    float u_0 = atof(argv[5]);
    float v_0 = atof(argv[6]);

    float alpha_u = atof(argv[7]);
    float alpha_v = atof(argv[8]);

    // Extrinsics
    float gama = atof(argv[9]);
    float beta = atof(argv[10]);
    float alpha = atof(argv[11]);
    float T_x = atof(argv[12]);
    float T_y = atof(argv[13]);
    float T_z = atof(argv[14]);

    // Use depth map
    int use_depth = atoi(argv[15]);

    // output pam instead of ppm
    int out_pam = 0;
    if (argc == 17)
        out_pam = atoi(argv[16]);

    // The output filename is generated from the input
    char output_filename[512];
    sprintf(output_filename,
            "results/%c_%.0f_%d_%d_%.1f_%.1f_%.2f_%.2f_%.1f_%.1f_%.1f_%.1f_%.1f_%.1f_%d.%s",
            off_filename[7],
            f, width, height, u_0, v_0, alpha_u, alpha_v,
            gama, beta, alpha, T_x, T_y, T_z,
            use_depth,
            out_pam ? "pam" : "ppm"); // extention based on the output type

    // Start processing
    // Read the pointcloud
    struct point3d *points;
    int N_v = 0;
    points = readOff(off_filename, &N_v);

    // and center it
    centerThePCL(points, N_v);

    // Regid transformation
    float transition_matrix[16];
    computeTrans(gama, beta, alpha, T_x, T_y, T_z, transition_matrix);

    float trans_x, trans_y, trans_z, trans_w;
    for (int i = 0; i < N_v; i++)
    {
        trans_x = transition_matrix[0] * points[i].x + transition_matrix[1] * points[i].y + transition_matrix[2] * points[i].z + transition_matrix[3];
        trans_y = transition_matrix[4] * points[i].x + transition_matrix[5] * points[i].y + transition_matrix[6] * points[i].z + transition_matrix[7];
        trans_z = transition_matrix[8] * points[i].x + transition_matrix[9] * points[i].y + transition_matrix[10] * points[i].z + transition_matrix[11];
        trans_w = transition_matrix[12] * points[i].x + transition_matrix[13] * points[i].y + transition_matrix[14] * points[i].z + transition_matrix[15];
        points[i].x = trans_x / trans_w;
        points[i].y = trans_y / trans_w;
        points[i].z = trans_z / trans_w;
    }

    // allocate output image and initialize with white colors
    ppm_file image;
    image.rows = height;
    image.cols = width;
    image.maxval = 255;
    image.magic_number = '6';

    // Allocate what you think you need  DONE

    image.pixmap = malloc(sizeof(pixel) * height * width);

    // prepare a depth buffer if needed
    float *depth;
    if (use_depth)
    {
        printf("Using Depth Buffer\n");
        depth = malloc(width * height * sizeof(float));
        for (int i = 0; i < width * height; i++)
            depth[i] = INFINITY;
    }

    int orthogonal = 0;
    if (f == 0)
    {
        printf("Using Orthogonal camera\n");
        orthogonal = 1;
    }

    // extra buffer used as an extra channel for transparcy (pam)
    int *tr_channel;
    if (out_pam)
    {
        printf("OUtputting a pam file\n");
        tr_channel = calloc(width * height, sizeof(int));
    }

    float X_cam, Y_cam;
    int x_u, y_u;
    // Go through the point-cloud
    for (int i = 0; i < N_v; i++)
    {
        // Project the point DONE
        if (orthogonal)
        {
            X_cam = points[i].x;
            Y_cam = points[i].y;
        }
        else
        {
            X_cam = points[i].x / (1.0f + points[i].z / f);
            Y_cam = points[i].y / (1.0f + points[i].z / f);
        }

        x_u = X_cam / alpha_u + u_0;
        y_u = Y_cam / alpha_v + v_0;

        // Check if the point is inside the image
        if (x_u >= width || y_u >= height || x_u < 0 || y_u < 0)
            continue;

        // Do something about the depth DONE

        if (use_depth)
        {
            if (points[i].z > depth[x_u + y_u * width])
                continue;
            depth[x_u + y_u * width] = points[i].z;
        }

        // If ok, update the image pixel color
        image.pixmap[x_u + y_u * width].blue = points[i].b;
        image.pixmap[x_u + y_u * width].red = points[i].r;
        image.pixmap[x_u + y_u * width].green = points[i].g;

        // setting it as not transparent
        if (out_pam)
            tr_channel[x_u + y_u * width] = 255;
    }

    // Save the image
    printf("Writting output file %s", output_filename);
    if (out_pam)
        write_pam(image, tr_channel, output_filename);
    else
        write_ppm(image, output_filename);
}