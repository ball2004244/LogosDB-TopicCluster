# Use the official Python 3.11 image as the base image
FROM python:3.11

# Set the working directory inside the container
WORKDIR /app

# Copy the necessary files to the working directory
COPY src/smart_query/database.py .
COPY src/smart_query/vect_query.py .
COPY src/smart_query/smart_query.py .

# Install the required dependencies
RUN pip install psycopg2-binary uvicorn fastapi numpy scikit-learn

# Expose port 8000
EXPOSE 8000

# Set the command to run the application
CMD [ "python3", "smart_query.py" ]