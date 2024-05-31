#include <iostream>
#include <fstream>
#include <vector>
#include <string>

/*
This file is used to generate a dynamic docker-compose file based on the input file and template file.
The number of created containers is equal to the number of lines in the input file.
The input file contains the names of the containers.
The template file contains the template of the services and volumes sections of the docker-compose file.
The output file is the generated docker-compose file.
*/
std::vector<std::string> readInputFile(const std::string &inputFileName)
{
    std::ifstream inputfile(inputFileName);
    if (!inputfile)
    {
        std::cerr << "Error: " << inputFileName << " could not be opened" << std::endl;
        exit(1);
    }

    std::vector<std::string> containers;
    std::string line;
    while (std::getline(inputfile, line))
    {
        containers.push_back(line);
    }
    inputfile.close();

    return containers;
}

std::pair<std::string, std::string> readTemplateFile(const std::string &templateFile)
{
    std::ifstream infile(templateFile);
    std::string serviceTemplate = "";
    std::string volumeTemplate = "";

    std::string line = "";
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
            serviceTemplate += line + "\n";
        }
        if (isVolumesSection)
        {
            volumeTemplate += line + "\n";
        }
    }

    return {serviceTemplate, volumeTemplate};
}

std::string generateConfig(const std::vector<std::string> &containers, 
                            const std::string &serviceTemplate, const std::string &volumeTemplate)
{
    std::string serviceBody = "services:\n";
    std::string volumeBody = "volumes:\n";
    std::string footer = "networks:\n  topicdb-cluster: {}\n";

    std::string jinja2Variable = "{{ container_name }}";
    for (const std::string &container : containers)
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

    return "version: '3'\n" + serviceBody + volumeBody + footer;
}

void writeOutputFile(const std::string &outputFile, const std::string &config)
{
    std::ofstream outfile(outputFile);
    outfile << config;
    outfile.close();
}

int main()
{
    std::string inputFileName = "topics.txt";
    std::string templateFile = "template.yml";
    std::string outputFile = "compose.yml";

    std::vector<std::string> containers = readInputFile(inputFileName);
    std::pair<std::string, std::string> templates = readTemplateFile(templateFile);
    std::string serviceTemplate = templates.first;
    std::string volumeTemplate = templates.second;
    std::string config = generateConfig(containers, serviceTemplate, volumeTemplate);
    writeOutputFile(outputFile, config);

    std::cout << outputFile << " has been created successfully!" << std::endl;

    return 0;
}