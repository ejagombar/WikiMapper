#!/usr/bin/env bash

set -e

# === CONFIG ===
APP_NAME="neo4j-server"
INSTALL_DIR="/opt/$APP_NAME"
ENV_FILE="/etc/default/$APP_NAME"
SERVICE_FILE="/etc/systemd/system/$APP_NAME.service"

echo "Building $APP_NAME..."
go build -o $APP_NAME main.go

echo "Creating $INSTALL_DIR..."
sudo mkdir -p $INSTALL_DIR
sudo cp ./$APP_NAME $INSTALL_DIR/
sudo chmod +x $INSTALL_DIR/$APP_NAME

echo "Setting up environment variables..."
if [ -f ".env" ]; then
    echo "Using .env file..."
    sudo cp .env $ENV_FILE
else
    echo "Creating $ENV_FILE..."
    cat <<EOF | sudo tee $ENV_FILE
NEO4J_URL=http://127.0.0.1:7474
NEO4J_USERNAME=neo4j
NEO4J_PASSWORD=
API_PORT=
EOF
    echo "Edit $ENV_FILE if needed: sudo nano $ENV_FILE"
fi

echo "Installing systemd service file..."

sed "s|EXEC_START_PLACEHOLDER|$INSTALL_DIR/$APP_NAME|g" ./$APP_NAME.service | sudo tee $SERVICE_FILE > /dev/null

echo "Reloading systemd..."
sudo systemctl daemon-reload

echo "Enabling $APP_NAME.service..."
sudo systemctl enable $APP_NAME.service

echo "Starting $APP_NAME.service..."
sudo systemctl restart $APP_NAME.service

echo "Done! Check status with: sudo systemctl status $APP_NAME"
echo "View logs with: sudo journalctl -u $APP_NAME -f"
