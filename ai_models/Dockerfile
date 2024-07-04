# Use an official Python runtime as a parent image
FROM python:3.10

# Set the working directory in the container
WORKDIR /app
COPY . /app

# Install necessary dependencies
RUN pip install --no-cache-dir fastapi uvicorn keybert

# Make port 8000 available to the world outside this container
EXPOSE 8000

# Run app.py when the container launches
CMD ["uvicorn", "kw_extract:app", "--host", "0.0.0.0", "--port", "8000"]
