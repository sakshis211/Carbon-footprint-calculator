#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cmath>

class EnterpriseCarbonCalculator {
public:
    // Scientifically grounded Emission factors (Kg CO2e per unit)
    const double EF_ELECTRICITY = 0.71;   // Grid average
    const double EF_LPG_KG = 2.98;        // Per kg of LPG
    const double EF_CAR = 0.192;          // Average mixed fuel car per km
    const double EF_BIKE = 0.08;          // Two-wheeler per km
    const double EF_METRO = 0.03;         // Public rail transit per km
    const double EF_BUS = 0.07;           // Public bus per km
    const double EF_FLIGHT = 0.25;        // Including radiative forcing per km
    const double EF_VEG_MEAL = 0.7;       // Plant-based meal average
    const double EF_NONVEG_MEAL = 2.5;    // Meat-based meal average
    const double EF_SCREEN_HR = 0.015;    // Device power + basic network
    const double EF_DATA_GB = 0.05;       // Data center & transmission

    double calculate(int people, std::string diet, int meals, double elec, double lpg, 
                     double screen, double data, double car, double bike, 
                     double metro, double bus, double flight) {
        
        // Household Energy (divided by occupants)
        double energy = ((elec * EF_ELECTRICITY) + (lpg * EF_LPG_KG)) / people;
        
        // Diet (Monthly = meals per day * 30 days)
        double diet_ef = (diet == "veg") ? EF_VEG_MEAL : EF_NONVEG_MEAL;
        double food = meals * diet_ef * 30;

        // Transport
        double personal_trans = (car * EF_CAR) + (bike * EF_BIKE);
        double public_trans = (metro * EF_METRO) + (bus * EF_BUS);
        double air_travel = (flight * EF_FLIGHT);

        // Digital
        double digital = (screen * EF_SCREEN_HR * 30) + (data * EF_DATA_GB);

        return energy + food + personal_trans + public_trans + air_travel + digital;
    }
};

int main(int argc, char* argv[]) {
    // Expecting 13 arguments: ProgramName + 12 variables
    if (argc != 13) {
        std::cerr << "{\"error\": \"Require exactly 12 inputs.\"}" << std::endl;
        return 1;
    }
    
    try {
        int people = std::max(1, std::stoi(argv[1]));
        std::string diet = argv[2];
        int meals = std::stoi(argv[3]);
        
        double elec = std::stod(argv[4]);
        double lpg = std::stod(argv[5]);
        double screen = std::stod(argv[6]);
        double data = std::stod(argv[7]);
        
        double car = std::stod(argv[8]);
        double bike = std::stod(argv[9]);
        double metro = std::stod(argv[10]);
        double bus = std::stod(argv[11]);
        double flight = std::stod(argv[12]);

        EnterpriseCarbonCalculator calc;
        double total_month = calc.calculate(people, diet, meals, elec, lpg, screen, data, car, bike, metro, bus, flight);
        
        // Determine Classification based on Indian Averages (approx 2.9 tonnes/year = ~240 kg/month)
        std::string title;
        if (total_month <= 150) title = "Eco Champion";         // Sustainable Indian Target
        else if (total_month <= 300) title = "Average Emitter"; // Around Indian average
        else title = "Heavy Emitter";                           // Above average for India

        // Determine Carbon Twin (Yearly tonnes mapping)
        double yearly_kg = total_month * 12;
        std::string twin = "USA "; 
        if (yearly_kg < 2500) twin = "Costa Rica ";
        else if (yearly_kg < 4500) twin = "Thailand ";
        else if (yearly_kg < 6500) twin = "United Kingdom ";
        
        // Equivalent cars running for a year (Average car emits ~4600 kg/year)
        double cars_equiv = yearly_kg / 4600.0;
        
        // Trees needed to offset (1 mature tree absorbs ~22kg per year)
        int trees_needed = std::ceil(yearly_kg / 22.0);

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "{"
                  << "\"total_emission\": " << total_month << ", "
                  << "\"title\": \"" << title << "\", "
                  << "\"twin\": \"" << twin << "\", "
                  << "\"cars\": " << cars_equiv << ", "
                  << "\"trees_needed\": " << trees_needed
                  << "}" << std::endl;
    } catch (...) {
        std::cerr << "{\"error\": \"Calculation failure.\"}" << std::endl;
        return 1;
    }
    return 0;
}