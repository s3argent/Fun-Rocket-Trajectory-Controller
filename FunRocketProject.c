#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#define MAX_GPS_BUFFER 128
#define GPS_TIMEOUT 10

typedef enum {
    GPS_UNKNOWN,
    GPS_UBLOX,
    GPS_SIRF,
    GPS_MTK,
    GPS_ANY
} GpsModel;

typedef struct {
    double lat;
    double lon;
    double alt;
    int sats;
    time_t timestamp;
} GpsData;

typedef struct {
    GpsModel model;
    char device[64];
    int fd;
    FILE* stream;
} GpsContext;

// GPS model detection
GpsModel detectGpsModel(const char* buffer) {
    if (strstr(buffer, "$GPGGA") || strstr(buffer, "$GPRMC")) return GPS_UBLOX;
    if (strstr(buffer, "$GNGGA") || strstr(buffer, "$GNRMC")) return GPS_SIRF;
    if (strstr(buffer, "$PMTK")) return GPS_MTK;
    return GPS_UNKNOWN;
}

// Read GPS data
GpsData readGpsData(GpsContext* ctx) {
    GpsData gps = {0};
    char buffer[MAX_GPS_BUFFER] = {0};
    time_t start_time = time(NULL);
    
    while ((time(NULL) - start_time) < GPS_TIMEOUT) {
        if (fgets(buffer, MAX_GPS_BUFFER, ctx->stream)) {
            switch (detectGpsModel(buffer)) {
                case GPS_UBLOX:
                    sscanf(buffer, "$GPGGA,%*s,%lf,%*s,%lf,%*s,%d", 
                           &gps.lat, &gps.lon, &gps.sats);
                    break;
                case GPS_SIRF:
                    sscanf(buffer, "$GNGGA,%*s,%lf,%*s,%lf,%*s,%d", 
                           &gps.lat, &gps.lon, &gps.sats);
                    break;
                case GPS_MTK:
                    sscanf(buffer, "$PMTK011,%*s,%lf,%*s,%lf,%*s,%d", 
                           &gps.lat, &gps.lon, &gps.sats);
                    break;
            }
            
            if (gps.lat && gps.lon) {
                gps.timestamp = time(NULL);
                return gps;
            }
        }
    }
    
    return gps;
}

// Calculate trajectory
void calculateTrajectory(GpsData current, GpsData target, double* distance, double* bearing) {
    double dLat = (target.lat - current.lat) * M_PI / 180;
    double dLon = (target.lon - current.lon) * M_PI / 180;
    
    *distance = 6371000 * acos(sin(current.lat*M_PI/180)*sin(target.lat*M_PI/180) + 
                               cos(current.lat*M_PI/180)*cos(target.lat*M_PI/180)*cos(dLon));
    
    double y = sin(dLon) * cos(target.lat*M_PI/180);
    double x = cos(current.lat*M_PI/180) * sin(target.lat*M_PI/180) - 
               sin(current.lat*M_PI/180) * cos(target.lat*M_PI/180) * cos(dLon);
    
    *bearing = atan2(y, x) * 180 / M_PI;
}

// Main function
int main(int argc, char** argv) {
    GpsContext ctx = {0};
    GpsData current, target;
    double distance, bearing;
    
    // Initialize GPS context
    strcpy(ctx.device, "/dev/ttyUSB0"); // Default USB GPS
    
    if (argc > 1) {
        strcpy(ctx.device, argv[1]); // Custom device path
    }
    
    ctx.stream = fopen(ctx.device, "r");
    if (!ctx.stream) {
        fprintf(stderr, "Failed to open GPS device: %s\n", ctx.device);
        return 1;
    }
    
    // Read initial GPS data
    printf("Detecting GPS model...\n");
    current = readGpsData(&ctx);
    if (!current.lat || !current.lon) {
        fprintf(stderr, "Failed to read GPS data\n");
        fclose(ctx.stream);
        return 1;
    }
    
    printf("Detected GPS model: %s\n", 
           ctx.model == GPS_UBLOX ? "UBLOX" : 
           ctx.model == GPS_SIRF ? "SIRF" : 
           ctx.model == GPS_MTK ? "MTK" : "Unknown");
    
    printf("Current position: %.6f,%.6f\n", current.lat, current.lon);
    
    // Get target coordinates
    printf("Enter target coordinates (lat lon): ");
    scanf("%lf %lf", &target.lat, &target.lon);
    
    // Calculate trajectory
    calculateTrajectory(current, target, &distance, &bearing);
    
    printf("\nTrajectory calculated:\n");
    printf("Distance: %.2fm\n", distance);
    printf("Bearing: %.2f°\n", bearing);
    
    fclose(ctx.stream);
    return 0;
}