import subprocess, os, csv, shutil

WEBOTS_HOME = "/usr/local/webots"
os.environ["WEBOTS_HOME"] = WEBOTS_HOME

if not os.path.exists("auto_output"):
    os.makedirs("auto_output")
if not os.path.exists("auto_input"):
    print("ERROR: auto_input directory does not exist")
    exit()

CWD = os.getcwd()
teams = os.listdir("auto_input")
teams.sort()

results = []

for team in teams:

    try:
        team_dir = os.path.join("auto_input", team, "controllers", team)
        files = os.listdir(team_dir)
        print(files)

        os.chdir(team_dir)

        if "Makefile" in files:
            process = subprocess.Popen(["make"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True)
            while True:
                output = process.stdout.readline()
                print(output)

                if process.poll() is not None:
                    break

            shutil.move(team, CWD+"/controllers/competitionController/competitionController")

    except:
        print("No controller")

    process = subprocess.Popen(["webots", "--mode=fast", "--minimize", "--no-rendering", "--stdout", "--stderr"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True)

    run = [team, -1, -1, "none"]
    while True:
        output = process.stdout.readline()
        print(output)

        idx = output.find("score: ")
        if idx != -1:
            run[1] = output[idx+7:-1]

        idx = output.find("time: ")
        if idx != -1:
            run[2] = output[idx+6:-1]
            

        if process.poll() is not None:

            print("Webots exit code: " + str(process.returncode))

            results.append(run)
            os.chdir(CWD)

            files = os.listdir("auto_output")
            if team not in files:
                os.makedirs("auto_output/"+team)

            shutil.move("worlds/results/result.csv", "auto_output/"+team+"/"+team+".csv")

            try:
                shutil.move("controllers/Game_Supervisor/movie.mp4", "auto_output/" + team + "/" +team  + ".mp4")
            except:
                print("ERROR: No video found")

            break


with open("auto_output/results.csv", mode='w') as results_file:
    writer = csv.writer(results_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
    writer.writerow(["TEAM", "SCORE", "TIME", "MESSAGE"])
    for row in results:
        writer.writerow(row)