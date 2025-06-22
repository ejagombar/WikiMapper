#!/usr/bin/env bash
set -e

APP_NAME="wikimapper-server"
INSTALL_DIR="/opt/$APP_NAME"
SERVICE_NAME="$APP_NAME.service"
SERVICE_FILE_SRC="./$SERVICE_NAME"
SERVICE_FILE_DST="/etc/systemd/system/$SERVICE_NAME"
ENV_FILE="/etc/default/$APP_NAME"

echo "Building Go binary..."
go build -o $APP_NAME main.go

echo "Installing binary to $INSTALL_DIR..."
sudo mkdir -p $INSTALL_DIR
sudo cp $APP_NAME $INSTALL_DIR/
sudo chown -R www-data:www-data $INSTALL_DIR
sudo chmod +x $INSTALL_DIR/$APP_NAME

echo "Creating env file at $ENV_FILE..."
sudo tee $ENV_FILE > /dev/null <<EOF
NEO4J_URL=http://127.0.0.1:7474
NEO4J_USERNAME=neo4j
NEO4J_PASSWORD=
API_PORT=
EOF

echo "Copying $SERVICE_NAME to systemd directory..."
sudo cp $SERVICE_FILE_SRC $SERVICE_FILE_DST

echo "Reloading systemd..."
sudo systemctl daemon-reload

echo "Enabling $APP_NAME to start on boot..."
sudo systemctl enable $APP_NAME

echo "Starting $APP_NAME..."
sudo systemctl restart $APP_NAME

echo "Done! Check status with: sudo systemctl status $APP_NAME"

