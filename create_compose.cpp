#include <iostream>
#include <fstream>
#include <vector>
#include <string>

int main()
{
    // Read the number of containers to be created
    std::string inputFileName = "input.txt";
    std::cout << "Read input array from file: " << inputFileName << std::endl;
    std::ifstream inputfile(inputFileName);
    if (!inputfile)
    {
        std::cerr << "Error: " << inputFileName << " could not be opened" << std::endl;
        return 1;
    }

    // Convert input content to vector
    std::vector<std::string> containers;
    std::string line;
    while (std::getline(inputfile, line))
    {
        containers.push_back(line);
    }
    inputfile.close();

    // Create compose.yml with dynamic number of containers
    std::string header = "version: '3'\n";
    std::string footer = "\n";
    std::string body = "";

    // Body will be dynamically created
    // For postgres, there are 2 sections: containers definition and volumes definition

    // First, load template for postgres
    std::string templateFile = "template.yml";
    std::ifstream infile(templateFile);

    std::string serviceTemplate = "";
    std::string volumeTemplate = "";

    // Read the template file by line and split by delimiter
    line = "";
    bool isServicesSection = false;
    bool isVolumesSection = false;
    while (std::getline(infile, line))
    {
        if (line == "services:")
        {
            isServicesSection = true;
            continue;
        }
        if (line == "volumes:")
        {
            isServicesSection = false;
            isVolumesSection = true;
            continue;
        }
        if (isServicesSection)
        {
            // Process services section
            serviceTemplate += line + "\n";
        }
        if (isVolumesSection)
        {
            // Process volumes section
            volumeTemplate += line + "\n";
        }
    }

    // Craft Dynamic Body, Replace all jinja2 variables
    std::string serviceBody = "services:\n";
    std::string volumeBody = "volumes:\n";
    std::string jinja2Variable = "{{ container_name }}";
    for (std::string container : containers)
    {
        std::string serviceBodyTemp = serviceTemplate;
        std::string volumeBodyTemp = volumeTemplate;
        size_t pos;
        while ((pos = serviceBodyTemp.find(jinja2Variable)) != std::string::npos)
            serviceBodyTemp.replace(pos, jinja2Variable.length(), container);
        while ((pos = volumeBodyTemp.find(jinja2Variable)) != std::string::npos)
            volumeBodyTemp.replace(pos, jinja2Variable.length(), container);
        serviceBody += serviceBodyTemp;
        volumeBody += volumeBodyTemp;
    }

    // Now craft config file
    std::string config = header + serviceBody + volumeBody + footer;

    // Save config to compose.yml
    std::string outputFile = "compose.yml";
    std::ofstream outfile(outputFile);
    outfile << config;
    outfile.close();

    std::cout << "compose.yml has been created successfully!" << std::endl;

    return 0;
}
