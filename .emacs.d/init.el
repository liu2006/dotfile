; -*- lexical-binding: t; -*-
(setq inhibit-startup-screen t)
(tool-bar-mode 0)
(menu-bar-mode 0)
(scroll-bar-mode 0)
(ido-mode 1)
(line-number-mode 1)
(column-number-mode 1)
(setq ido-enable-flex-matching t)
(setq ido-everywhere t)
(global-display-line-numbers-mode 1)
(setq-default display-line-numbers-type 'relative)
(setq ring-bell-function 'ignore)
(set-face-attribute 'default nil
                    :family "AurulentSansMNerdFontMono"
                    :height 150
                    :width 'regular)

(setq c-ts-indent-offset 4)
(require 'ansi-color)
(add-hook 'compilation-filter-hook
          #'ansi-color-compilation-filter)

;; Install vterm package
(unless (package-installed-p 'vterm)
  (package-vc-install
   '(vterm :url "https://github.com/emacsmirror/vterm.git")))
(setq vterm-timer-delay nil)

;; Install the dependency f package for doom-modeline
(unless (package-installed-p 'f)
  (package-vc-install
   '(f :url "https://github.com/rejeep/f.el.git")))
(add-to-list 'load-path "~/.emacs.d/elpa/f")

;; Install the dependency s package for doom-modeline
(unless (package-installed-p 's)
  (package-vc-install
   '(s :url "https://github.com/magnars/s.el.git")))
(add-to-list 'load-path "~/.emacs.d/elpa/s")

;; Install the dependency shrink-path package for doom-modeline
(unless (package-installed-p 'shrink-path)
  (package-vc-install
   '(shrink-path :url "https://github.com/emacsmirror/shrink-path.git")))
(add-to-list 'load-path "~/.emacs.d/elpa/shrink-path")

;; Install modeline package
(unless (package-installed-p 'doom-modeline)
  (package-vc-install
   '(doom-modeline :url "https://github.com/seagle0128/doom-modeline.git")))
(add-to-list 'load-path "~/.emacs.d/elpa/doom-modeline")
(require 'doom-modeline)
(setq nerd-icons-font-family "AurulentSansMNerdFont")
(doom-modeline-mode 1)

;; Install slang-mode package
(unless (package-installed-p 'slang-mode)
  (package-vc-install
   '(slang-mode :url "https://github.com/K1ngst0m/slang-mode.git")))
(require 'slang-mode)
(add-to-list 'auto-mode-alist '("\\.slang\\'" . slang-mode))

;; Install color-theme-sanityinc-tomorrow package
(unless (package-installed-p 'color-theme-sanityinc-tomorrow)
  (package-vc-install
   '(color-theme-sanityinc-tomorrow :url "https://github.com/purcell/color-theme-sanityinc-tomorrow.git")))
(load-theme 'sanityinc-tomorrow-night t)

;;Install smex package
(unless (package-installed-p 'smex)
  (package-vc-install
   '(smex :url "https://github.com/nonsequitur/smex.git")))
(require 'smex)
(smex-initialize)
(global-set-key (kbd "M-x") 'smex)
(global-set-key (kbd "M-X") 'smex-major-mode-commands)

;; Install company-mode package
(unless (package-installed-p 'company-mode)
  (package-vc-install 
   '(company-mode :url "https://github.com/company-mode/company-mode.git")))
(require 'company)
(add-hook 'after-init-hook 'global-company-mode)
(setq company-idle-delay nil)
(global-set-key (kbd "M-TAB") 'company-complete)

;; Install magit package
(unless (package-installed-p 'magit)
  (package-vc-install
   '(magit :url "https://github.com/magit/magit.git")))
(require 'magit)

;; Install multiple-cursor package
(unless (package-installed-p 'multiple-cursors)
  (package-vc-install
   '(multiple-cursors :url "https://github.com/magnars/multiple-cursors.el.git")))
(require 'multiple-cursors)
(global-set-key (kbd "C-S-c C-S-c") 'mc/edit-lines)
(global-set-key (kbd "C->") 'mc/mark-next-like-this)
(global-set-key (kbd "C-<") 'mc/mark-previous-like-this)
(global-set-key (kbd "C-c C-<") 'mc/mark-all-like-this)

;; Install treesit-auto package
(unless (package-installed-p 'treesit-auto)
  (package-vc-install
   '(treesit-auto :url "https://github.com/renzmann/treesit-auto.git")))
(require 'treesit-auto)
(treesit-auto-add-to-auto-mode-alist 'all)
(global-treesit-auto-mode 1)
(setq treesit-auto-install 't)

(setq major-mode-remap-alist
      '((yaml-mode . yaml-ts-mode)
	(html-mode . html-ts-mode)
	(sh-mode . bash-ts-mode)
	(c-mode . c-ts-mode)
	(c++-mode . c++-ts-mode)
	(json-mode . json-ts-mode)
	(cmake-mode . cmake-ts-mode)
	(css-mode . css-ts-mode)))

(customize-set-variable 'treesit-font-lock-level 4)

(with-eval-after-load 'eglot
  (add-to-list 'eglot-server-programs
               '((c-ts-mode c++-ts-mode) . ("clangd")))
  (add-to-list 'eglot-server-programs
               '(bash-ts-mode . ("bash-language-server")))
  (add-to-list 'eglot-server-programs
               '(cmake-ts-mode . ("cmake-language-server")))
  (add-to-list 'eglot-server-programs
               '(yaml-ts-mode . ("yaml-language-server")))
  (add-to-list 'eglot-server-programs
               '(json-ts-mode . ("vscode-langservers-extracted")))
  (add-to-list 'eglot-server-programs
               '(css-ts-mode . ("vscode-langservers-extracted")))
  (add-to-list 'eglot-server-programs
               '(html-ts-mode . ("vscode-langservers-extracted")))
  (add-to-list 'eglot-server-programs
               '(slang-mode . ("slangd")))
  )

(add-hook 'c-ts-mode-hook 'eglot-ensure)
(add-hook 'c++-ts-mode-hook 'eglot-ensure)
(add-hook 'cmake-ts-mode-hook 'eglot-ensure)
(add-hook 'css-ts-mode-hook 'eglot-ensure)
(add-hook 'html-ts-mode-hook 'eglot-ensure)
(add-hook 'json-ts-mode-hook 'eglot-ensure)
(add-hook 'yaml-ts-mode-hook 'eglot-ensure)
(add-hook 'slang-mode-hook 'eglot-ensure)
(add-hook 'make-ts-mode-hook 'eglot-ensure)

(add-hook 'eglot-managed-mode-hook
          (lambda ()
            (eglot-inlay-hints-mode 0)))

(custom-set-variables
 ;; custom-set-variables was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(package-selected-packages '(treesit-auto))
 '(package-vc-selected-packages
   '((treesit-auto :url "https://github.com/renzmann/treesit-auto.git")
     (shrink-path :url
				  "https://github.com/emacsmirror/shrink-path.git"))))
(custom-set-faces
 ;; custom-set-faces was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 )
(put 'dired-find-alternate-file 'disabled nil)

