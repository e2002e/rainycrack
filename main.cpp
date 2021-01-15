#include <cstdlib>
#include <cstdbool>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <cmath>
#include <signal.h>
#include <thread>
#include <mutex>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Input.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Timer.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Scroll.H>

#include "generateur.h"
#include "cracker.h"
#include "pot.h"

bool stop;
bool addnl;
std::mutex c, p;
int mt;
int wwidth = Fl::w() / 2;
int wheight = Fl::h() / 2;
	

class Ui_options {
	public:
	int min;
	int max;
	char *set;
	char *hashfile;
	bool crack;
	bool restore;
};

Generateur *generateur;
Cracker *cracker;
Pot *pot;
Ui_options *options;

Fl_Spinner *spin0;
Fl_Spinner *spin1;
Fl_Input *in;
Fl_Button *rbutton;
Fl_Button *pbutton;
Fl_Button *cbutton;
Fl_File_Input *file;

//Status
Fl_Progress *progress;
Fl_Multiline_Output *output;
uint_big Total;

extern uint_big powi(uint32_t b, uint32_t p);

void get_status(void *) {
	progress->value((float) generateur->Counter /  (float) Total * 100.0f);
	Fl::repeat_timeout(2.0, get_status);
}

void generate(int t) {	
	/*we are looping on the different lengths twice, once outside and once inside the main iteration for which we tweak
	the bound with subtotal*/
	int mmm = generateur->max-generateur->min;
	for(generateur->loop[t]=generateur->L[t]; generateur->loop[t] <= mmm; ++generateur->loop[t]) {
		int mpl = generateur->min+generateur->loop[t];

		uint_big total;

		if(generateur->min == 1 && generateur->loop[t] == 0) {
			if(t == 0)
				total = generateur->length;
			else total = 0;
		}
		else {
			total = powi(generateur->length, mpl);
			if(t == 0) {
				total /= mt;
				total += powi(generateur->length, mpl) % mt;
			}
			else total /= mt;
		}
		uint_big subtotal = 0;
		if(generateur->loop[t] > 0) {
			if(generateur->min == 1 && generateur->loop[t] == 1) {
				if(t == 0)
					subtotal = generateur->length;
				else subtotal = 0;
			}
			else {
				subtotal = powi(generateur->length, mpl-1);
				if(t == 0) {
					subtotal /= mt;
					subtotal += powi(generateur->length, mpl-1) % mt;
				}
				else subtotal /= mt;
			}
		}
		char word[generateur->max];
		for(generateur->a[t] = generateur->A[t]; generateur->a[t] < (total - subtotal); ++generateur->a[t]) {
			//s.lock();
			if(stop) {
				generateur->save();
				goto end;
			}
			//s.unlock();
			//the inner loop on the lengths
			for(int loop2 = generateur->loop[t]; loop2 <= mmm; ++loop2) {
				//s1.lock();
				if(stop) {
					generateur->save();
					goto end;
				}
				memset(word, 0, sizeof(word));
				//s1.unlock();
				generateur->gen_tacking(t, loop2, word);
				
				if(cracker->crack) {
					if(cracker->hash_check(word)) {
						stop = true;
						p.lock();
						progress->value(100.0f);
						p.unlock();
					}						
				}
				else
					printf("%s\n", word);
				c.lock();
				generateur->Counter++;
				c.unlock();
			}
		}
	}
	end:
	;
}

//joining inside the button callback locks the ui
void generate_wrapper(Fl_Widget *widget) {
	stop = false;
	widget->deactivate();
	Fl::add_timeout(2.0, get_status);
	if(cracker->crack) {
		for(int t=0; t<mt; t++) {
			std::thread th(generate, t);
			th.join();
		}
	}
	else generate(0);
	
	if(!stop)//when we force the progress to 100%, stop is set, so don't overwrite it.
		get_status(NULL);
	output->redraw();
	progress->redraw();

	Fl::remove_timeout(get_status, 0);
	widget->activate();
	stop = true;

	delete generateur;
	delete cracker;
	delete pot;
}
void set_restore(Fl_Widget *widget, void *) {
	Fl_Button *b = (Fl_Button *) widget;
	if(b->value()) {
		options->restore = true;
	}
	else {
		options->restore = false;
	}
}

void run_button(Fl_Widget *widget, void *) {
	generateur = new Generateur;
	cracker = new Cracker;
	pot = new Pot;
	
	if(options->restore) {
		if(generateur->restore())
			return;		
	}
	else {
		generateur->min = options->min;
		generateur->max = options->max;
		generateur->length = strlen(options->set);
		
		if(generateur->length % 2) {
			fl_message("Tacking doesn't work with odd character set's length");
			return;
		}
		cracker->crack = options->crack;
		if(cracker->crack) {
			mt = std::thread::hardware_concurrency();
			cracker->filename = new char[strlen(file->value())];
			memset(cracker->filename, '\0', sizeof(cracker->filename));
			strcpy(cracker->filename, file->value());
			if(cracker->filename == NULL) {
				delete cracker->filename;
				return;
			}
			if(cracker->import_hashes())
				return;//memory will be freed inside this function if failure occurs
		}
		else mt = 1;
		//from here there is no input error possible
		generateur->arrayofchars = new char[generateur->length];
		strcpy(generateur->arrayofchars, options->set);
		generateur->split_work();
	}
	generateur->loop = new int [mt];
	generateur->a = new uint_big [mt];
	//not setting these will write wrong values in the restore file if no multithreading is used.
	int mmm = generateur->max-generateur->min;
	for(int t=0; t<mt; t++) {
		generateur->loop[t] = 0;
		generateur->a[t] = 0;
	}
	Total = 0;
	for(int c=0; c<=mmm; ++c)
		Total += powi(generateur->length, c+generateur->min);

	std::thread th(generate_wrapper, widget);
	th.detach();
}
void halt_button(Fl_Widget *widget, void *) {
	stop = true;
}

void set_minlength(Fl_Widget *spin, void *) {
	Fl_Spinner *spinner = (Fl_Spinner*) spin;
	options->min = (int) spinner->value();
}

void set_maxlength(Fl_Widget *spin, void *) {
	Fl_Spinner *spinner = (Fl_Spinner*) spin;
	options->max = (int) spinner->value();
}

void set_set(Fl_Widget *in, void *) {
	Fl_Input * input = (Fl_Input*) in;
	delete options->set;
	options->set = new char[strlen(input->value())];
	strcpy(options->set, input->value());
}

void set_crack(Fl_Widget *button, void *) {
	Fl_Button *b = (Fl_Button *) button;
	if(b->value()) {
		options->crack = true;
	}
	else {
		options->crack = false;
	}
}

int main(int argc, char *argv[]) {
	int arg;

	stop = true;
	addnl = false;
	
	options = new Ui_options;
	options->crack = false;
	options->restore = false;
	Fl_Window *window = new Fl_Window(wwidth, wheight);

	//Set default for bare necessary
	spin0 = new Fl_Spinner(wwidth/32, wheight/16, wwidth/2-wwidth/16, wheight/16, "Min");
	spin0->value(1);
	spin0->minimum(1);
	spin0->maximum(32);
	options->min = 1;
	spin1 = new Fl_Spinner(wwidth/2+wwidth/32, wheight/16, wwidth/2-wwidth/16, wheight/16, "Max");
	spin1->value(6);
	spin1->minimum(1);
	spin1->maximum(32);
	options->max = 6;
	spin0->align(FL_ALIGN_TOP);
	spin1->align(FL_ALIGN_TOP);
	spin0->callback(set_minlength);
	spin1->callback(set_maxlength);

	in = new Fl_Input(wwidth/32, wheight/16*2+wheight/32, wwidth-wwidth/16, wheight/8, "Set");
	//in->value("aeorisn1tl2md0cp3hbuk45g9687yfwjvzxq");//ASERBTMLNPOIDCHGKFJUWYVZQX
	in->value("1234567890");
	in->align(FL_ALIGN_TOP);
	in->callback(set_set);

	options->set = new char[strlen(in->value())];
	strcpy(options->set, in->value());

	progress = new Fl_Progress(wwidth/32, wheight/4+wheight/16, wwidth-wwidth/16, wheight/32);
	progress->selection_color(FL_BLUE);
	progress->minimum(0);
	progress->maximum(100);

	Fl_Button *run = new Fl_Button(wwidth/32, wheight/4 + wheight/8, wwidth/2-wwidth/16, wheight/12, "Run");
	Fl_Button *halt = new Fl_Button(wwidth/2+wwidth/32, wheight/4 + wheight/8, wwidth/2-wwidth/16, wheight/12, "Stop");
	run->callback(run_button);
	halt->callback(halt_button);

	file = new Fl_File_Input(wwidth/32, wheight/2, wwidth-wwidth/16, wheight/12, "Hash file's path:");
	file->align(FL_ALIGN_TOP);

	rbutton = new Fl_Button(wwidth/32, wheight/2+wheight/8, wwidth/2-wwidth/16, wheight/12, "Restore");
	rbutton->type(FL_TOGGLE_BUTTON);
	rbutton->callback(set_restore);

	cbutton = new Fl_Button((wwidth/2+wwidth/32), wheight/2+wheight/8, wwidth/2-wwidth/16, wheight/12, "Hash attack");
	cbutton->type(FL_TOGGLE_BUTTON);
	cbutton->callback(set_crack);
	
	output = new Fl_Multiline_Output(wwidth/32, wheight/2+wheight/4, wwidth-wwidth/16, wheight/4-wheight/32);
	output->align(FL_ALIGN_BOTTOM);
	
	window->end();
	window->show(argc, argv);
	return Fl::run();
}

