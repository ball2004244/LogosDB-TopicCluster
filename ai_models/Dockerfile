# Use an official Python runtime as a parent image
FROM python:3.10

# Set the working directory in the container
WORKDIR /app
COPY ai_models/ .

# Install necessary dependencies
RUN pip install fastapi uvicorn keybert

# Run app.py when the container launches
CMD ["uvicorn", "kw_extract:app", "--host", "0.0.0.0", "--port", "8000"]
